/*
 * Copyright 2022, Dominic Martinez, dom@dominicm.dev.
 * Distributed under the terms of the MIT License.
 */

#include "Piecemeal.hpp"

#include "code/Leaf.hpp"
#include "data/Target.hpp"
#include "make/Command.hpp"
#include "make/MakeException.hpp"

#include <cmath>
#include <sstream>
#include <vector>

namespace ham::make
{

data::StringListList
Piecemeal::Words(
	code::EvaluationContext context,
	std::vector<std::pair<std::string_view, std::string_view>> words,
	StringList boundSources,
	std::size_t maxLine
)
{
	/* Calculate the size of the command with a given source list to split
	 * commands up with the `piecemeal` modifier.
	 *
	 * To calculate the size of the command, split the command into
	 * whitespace-separated words, calculate the size of each word, and then sum
	 * the total of word and whitespace.
	 *
	 * To calculate the size of a word:
	 *
	 * Let `eval` represent evaluating the word with a given source set.
	 *
	 * Let word groups be all the words generated by a specific combination of
	 * source elements (i.e. the constant words generated by the product of
	 * other variables).
	 *
	 * Let `length1 := eval{"a"}` and `length2 := eval{"a", "b"}`. The source
	 * _power_, or how many times the source variable is included in the word,
	 * is `log2(length2/length1)`. Because other variables are constant, this
	 * accounts for nested variables holding the source variable.
	 *
	 * Let `length1 := eval{"a"}` and `length2 := eval{"ab"}`. `length2 -
	 * length1 - power + 1` is the source _multiplicity_ (m), or how many times
	 * each source is included in the word because of external variable
	 * products. `length1 - (length2 - length1) = 2*length1 - length2` is also
	 * the base length of the word group.
	 *
	 * Say we are given the length of a word with a source set of length `L`.
	 * Let `A` be the average length of the sources in the set. We add a new
	 * source of length `S`
	 *
	 * Note: the below computation is mathematically correct when A is a
	 * rational number. For computational purposes, we store A as a float, and
	 * round up to get a good-enough answer.
	 *
	 * To add a source, consider the newly generated word groups where there are
	 * n <= p occurences of the new source. The number of such cases is
	 * C(n,p)*L^(p-n). The length of a case is `baseLength + m*n*S +
	 * otherSources`. When summing all the cases, each source appears in the sum
	 * the same number of times, so we can substitute the average for each
	 * occurence of an unknown source:
	 *
	 * Length = C(n,p)*L^(p-n)*(b + m*n*S + m*(p-n)*A)
	 *
	 * Then, sum n from 1 to p to obtain the length of all word groups added by
	 * the new source.
	 *
	 * Finally, to iterate, update the total length and average:
	 *
	 * newA = (oldA*L + newL)/(L + 1)
	 *
	 * The empty set has length 0, so via induction we can build up to any
	 * source set.
	 */
	data::StringListList piecemealSources{};

	// Helpers
	const auto genDomain = [context](std::vector<const char*> vars) {
		StringList list{};
		for (auto var : vars) {
			list.Append(var);
		}

		// Inherit from existing domain
		data::VariableDomain domain{*(context.BuiltInVariables())};
		domain.Set("2", list);
		domain.Set(">", list);
		return domain;
	};

	auto oldDomain = context.BuiltInVariables();

	// Includes trailing space for length to be consistent
	const auto getLength =
		[&oldDomain,
		 &context](data::VariableDomain* domain, std::string_view word) {
		context.SetBuiltInVariables(domain);
		const StringList list = code::Leaf::EvaluateString(
			context,
			word.cbegin(),
			word.cend(),
			nullptr
		);
		context.SetBuiltInVariables(oldDomain);

		std::size_t length = 0;
		for (std::size_t i = 0; i < list.Size(); i++) {
			// Add joined spaces
			length += list.ElementAt(i).Length() + 1;
		}
		return length;
	};

	const auto fact = [](std::size_t num) {
		if (num <= 1)
			return std::size_t{1};

		for (std::size_t i = num - 1; num > 1; num--) {
			num *= i;
		}

		return num;
	};

	const auto comb = [fact](std::size_t r, std::size_t n) {
		return fact(n) / (fact(r) * fact(n - r));
	};

	// Test variable domains
	auto singleDomain = genDomain({"a"});
	auto longDomain = genDomain({"ab"});
	auto dualDomain = genDomain({"a", "b"});

	// Calculate basic word info
	std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> wordInfo{};
	std::size_t baseCommandSize = 0;
	for (const auto& [word, space] : words) {
		const std::size_t singleLength = getLength(&singleDomain, word);
		const std::size_t longLength = getLength(&longDomain, word);
		const std::size_t dualLength = getLength(&dualDomain, word);

		if (singleLength == 0 || longLength == 0 || dualLength == 0) {
			throw MakeException("Failed to calculate word length");
		}

		if (dualLength % singleLength != 0)
			throw MakeException("Impossible variable expansion length");

		const std::size_t baseLength = 2 * singleLength - longLength;
		const std::size_t power =
			std::round(std::log2((double)dualLength / singleLength));
		const std::size_t multiplicity = longLength - singleLength - power + 1;

		// Count whitespace as constant
		baseCommandSize += space.length();

		// If word is a constant, add length directly to command
		if (power == 0) {
			baseCommandSize += baseLength - 1;
		} else {
			wordInfo.push_back({baseLength, power, multiplicity});
		}
	}

	// Piecemeal sources
	StringList sources{};
	double averageLength = 0;
	std::size_t commandSize = baseCommandSize;
	for (std::size_t i = 0; i < boundSources.Size(); i++) {
		auto source = boundSources.ElementAt(i);

		// Calculate size of added source
		for (const auto& [baseLength, power, multiplicity] : wordInfo) {
			// Calculate word size
			std::size_t wordSize = 0;
			for (std::size_t n = 1; n <= power; n++) {
				const std::size_t numGroups =
					comb(n, power) * std::pow(sources.Size(), power - n);
				const std::size_t newSourceLength =
					multiplicity * n * source.Length();
				const std::size_t otherSourceLength =
					std::round(multiplicity * (power - n) * averageLength);

				wordSize += numGroups
					* (baseLength + newSourceLength + otherSourceLength);
			}

			commandSize += wordSize;
		}

		// Each (non-constant) word has a trailing space that is included for
		// the incremental length calculation, but is not included in the final
		// command.
		if (commandSize - wordInfo.size() > maxLine) {
			if (sources.IsEmpty()) {
				std::stringstream error;
				error << "maxline of " << maxLine
					  << " is too small; unable to add source "
					  << source.ToStlString();
				throw MakeException(error.str());
			}

			piecemealSources.push_back(sources);
			sources = {};
			averageLength = 0;
			commandSize = baseCommandSize;
			i--; // redo calculation with current source
		} else {
			averageLength = (sources.Size() * averageLength + source.Length())
				/ (sources.Size() + 1);
			sources.Append(source);
		}
	}

	if (!sources.IsEmpty())
		piecemealSources.push_back(sources);

	return piecemealSources;
}

} // namespace ham::make
