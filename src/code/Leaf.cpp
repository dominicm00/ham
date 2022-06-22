/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "code/Leaf.hpp"

#include <algorithm>
#include <limits>
#include <utility>

#include "code/DumpContext.hpp"
#include "code/EvaluationContext.hpp"
#include "data/StringListOperations.hpp"

namespace ham
{
namespace code
{

Leaf::Leaf(const String& string)
	: fString(string)
{
}

Leaf::~Leaf() {}

StringList
Leaf::Evaluate(EvaluationContext& context)
{
	const char* string = fString.ToCString();
	return EvaluateString(context, string, string + fString.Length(), &fString);
}

code::Node*
Leaf::Visit(NodeVisitor& visitor)
{
	if (visitor.VisitNode(this))
		return this;

	return nullptr;
}

void
Leaf::Dump(DumpContext& context) const
{
	context << "Leaf(\"" << fString << "\")\n";
}

/*static*/ StringList
Leaf::EvaluateString(EvaluationContext& context,
					 const char* stringStart,
					 const char* stringEnd,
					 const String* originalString)
{
	// The string to evaluate is a alternating sequence of literal strings and
	// variable expansion expressions. Each literal string can be considered a
	// single element string list and each variable expansion expression expands
	// to a string list as well. The product of the list of all string lists is
	// the result we want to compute. We proceed accordingly, i.e. split the
	// input string in literal strings and variable expansion expressions,
	// evaluate the latter as we go, and finally compute the string list
	// product. Recursive variable expansion expressions we evaluate using
	// recursion.
	StringListList resultFactors;

	const char* literalStringStart = stringStart;
	const char* stringRemainder = literalStringStart;

	while (stringRemainder != stringEnd) {
		// find the next "$("
		stringRemainder = std::find(stringRemainder, stringEnd, '$');
		if (stringRemainder == stringEnd)
			break;

		if (++stringRemainder == stringEnd)
			break;

		if (*stringRemainder != '(')
			continue;

		// Add the literal string segment before the current variable to the
		// result factors.
		if (literalStringStart < stringRemainder - 1) {
			resultFactors.push_back(
				StringList(String(literalStringStart,
								  stringRemainder - 1 - literalStringStart)));
		}

		const char* variableStart = ++stringRemainder;

		// Find the matching closing ")". While at it also find the containing
		// special characters (":", "[", "]") at the top level.
		std::vector<const char*> colons;
		const char* openingBracket = nullptr;
		const char* closingBracket = nullptr;
		bool recursive = false;
		int matchCount = 1;
		while (matchCount != 0 && stringRemainder != stringEnd) {
			switch (*stringRemainder) {
				case '$':
					if (stringRemainder + 1 != stringEnd
						&& stringRemainder[1] == '(') {
						matchCount++;
						recursive = true;
					}
					break;
				case ')':
					matchCount--;
					break;
				case ':':
					if (matchCount == 1)
						colons.push_back(stringRemainder);
					break;
				case '[':
					if (matchCount == 1 && openingBracket == nullptr)
						openingBracket = stringRemainder;
					break;
				case ']':
					if (matchCount == 1 && closingBracket == nullptr)
						closingBracket = stringRemainder;
					break;
				default:
					break;
			}
			stringRemainder++;
		}

		if (matchCount != 0) {
			// TODO: Syntax error!
			break;
		}

		// Evaluate the variable. If its value is empty, the end result will be
		// empty, too.
		StringList variableValue =
			_EvaluateVariableExpression(context,
										variableStart,
										stringRemainder - 1,
										colons,
										openingBracket,
										closingBracket,
										recursive);
		if (variableValue.IsEmpty())
			return variableValue;

		resultFactors.push_back(variableValue);

		literalStringStart = stringRemainder;
	}

	// If we haven't encountered any variable, just return the original string.
	if (resultFactors.empty()) {
		if (originalString != nullptr)
			return StringList(*originalString);
		return StringList(String(stringStart, stringEnd - stringStart));
	}

	// Add the literal string segment after the last variable to the result
	// factors.
	if (literalStringStart != stringEnd) {
		resultFactors.push_back(StringList(
			String(literalStringStart, stringEnd - literalStringStart)));
	}

	// compute the result
	return StringList::Multiply(resultFactors);
}

/*static*/ StringList
Leaf::_EvaluateVariableExpression(EvaluationContext& context,
								  const char* variableStart,
								  const char* variableEnd,
								  const std::vector<const char*>& colons,
								  const char* openingBracket,
								  const char* closingBracket,
								  bool recursive)
{
	// The syntax is:
	//
	// expansion			:= variableName [ "[" elementRange "]" ]
	//							( ":" variableModifiers )*
	// elementRange			:= elementIndex [ "-" [ elementIndex ] ]
	// variableModifiers	:= variableSelector* [ variableSubstitution ]
	// variableSelector		:= "B" | "S" | "M" | "D" | "P" | "G" | "U" | "L"
	// variableSubstitution	:= variableSubstitutor "=" value
	// variableSubstitutor	:= "G" | "D" | "B" | "S" | "M" | "R" | "E" | "J"
	//
	// variableName, elementRange and variableModifiers are subject to variable
	// expansion. Each can result in a string list with more than one element.
	// The usual list multiplication rules apply, i.e.:
	// <variable1 range1 modifiers1> <variable1 range1 modifiers2> ...
	// <variable1 range2 modifiers1> ... <variable2 range1 modifiers1> ...
	//
	// Note: We're more lenient than jam, allowing ':' as a separator after
	// path part selectors. E.g. we allow "$(foo:G:B)", which jam considers
	// invalid syntax and ignores ":B".

	const char* variableNameEnd = variableEnd;
	const char* firstColon = colons.empty() ? nullptr : colons[0];

	if (firstColon != nullptr) {
		// Ignore brackets after the first colon.
		if (openingBracket != nullptr && firstColon < openingBracket)
			openingBracket = nullptr;
		if (closingBracket != nullptr && firstColon < closingBracket)
			closingBracket = nullptr;

		variableNameEnd = firstColon;
	}

	if (openingBracket != nullptr || closingBracket != nullptr) {
		// If we only have a closing bracket, consider the expression invalid
		// and return an empty list.
		if (openingBracket == nullptr)
			return StringList();

		// If the closing bracket is missing, we use the next "natural
		// boundary", i.e. the first colon or the variable end.
		if (closingBracket == nullptr)
			closingBracket = firstColon != nullptr ? firstColon : variableEnd;

		variableNameEnd = openingBracket;
	}

	if (variableStart == variableNameEnd)
		return StringList();

	// Handle the common cases first, i.e. no recursive expansion.
	if (!recursive) {
		StringList variableValue = context.LookupVariable(
			String(variableStart, variableNameEnd - variableStart));

		// range subscripts
		// The logical implementation would be to just limit variableValue to
		// the sublist specified by the subscripts. But Jam implements them
		// differently: The first subscript is applied, but from the second
		// subscript a maximum list size is computed, which is applied only
		// after the "E=..." operation has been applied.
		size_t maxSize;
		if (openingBracket != nullptr) {
			size_t firstIndex;
			size_t endIndex;
			if (!_ParseSubscripts(openingBracket + 1,
								  closingBracket,
								  firstIndex,
								  endIndex)) {
				return StringList();
			}

			if (firstIndex > 0) {
				variableValue =
					variableValue.SubList(firstIndex,
										  std::numeric_limits<size_t>::max());
			}

			maxSize = endIndex > firstIndex ? endIndex - firstIndex : 0;
		} else
			maxSize = std::numeric_limits<size_t>::max();

		// colon
		if (firstColon != nullptr) {
			data::StringListOperations operations;
			const char* colon = firstColon;
			std::vector<const char*>::const_iterator colonIt = colons.begin();
			for (;;) {
				++colonIt;
				const char* colonEnd =
					colonIt != colons.end() ? *colonIt : variableEnd;
				operations.Parse(colon + 1, colonEnd);
				if (colonEnd == variableEnd)
					break;

				colon = colonEnd;
			}

			variableValue =
				operations.Apply(variableValue, maxSize, context.GetBehavior());
		} else if (maxSize < variableValue.Size())
			variableValue = variableValue.SubList(0, maxSize);

		return variableValue;
	}

	// Handle the general, recursive case.

	// Expand the variable names.
	StringList variableNames =
		EvaluateString(context, variableStart, variableNameEnd, nullptr);
	size_t variableCount = variableNames.Size();

	// Expand and parse the subscripts.
	std::vector<std::pair<size_t, size_t>> subscripts;
	if (openingBracket != nullptr) {
		StringList subscriptStrings =
			EvaluateString(context, openingBracket + 1, closingBracket, nullptr);
		if (subscriptStrings.IsEmpty())
			return StringList();

		size_t subscriptsCount = subscriptStrings.Size();
		for (size_t subscriptsIndex = 0; subscriptsIndex < subscriptsCount;
			 subscriptsIndex++) {
			size_t firstIndex;
			size_t endIndex;
			String subscriptString =
				subscriptStrings.ElementAt(subscriptsIndex);
			if (!_ParseSubscripts(subscriptString.ToCString(),
								  subscriptString.ToCString()
									  + subscriptString.Length(),
								  firstIndex,
								  endIndex)) {
				return StringList();
			}
			subscripts.push_back(std::make_pair(firstIndex, endIndex));
		}
	} else {
		subscripts.push_back(
			std::make_pair(size_t(0), std::numeric_limits<size_t>::max()));
	}
	size_t subscriptsCount = subscripts.size();

	// Expand the operations. This is a bit more involved, since we can't just
	// expand first and parse then, as the expansion might introduce colons
	// which should not be treated as separator. So we expand each segment
	// individually. We create a list of expanded segments and then recursively
	// parse the operations.
	std::vector<StringList> operationsStringsList;
	// referenced by operationsList, so it needs to exist at least as long
	std::vector<data::StringListOperations> operationsList;
	if (firstColon != nullptr) {
		const char* segmentStart = firstColon + 1;
		std::vector<const char*>::const_iterator colonIt = colons.begin();
		for (;;) {
			++colonIt;
			const char* segmentEnd =
				colonIt != colons.end() ? *colonIt : variableEnd;
			StringList operationsStrings =
				EvaluateString(context, segmentStart, segmentEnd, nullptr);
			if (operationsStrings.IsEmpty())
				return StringList();

			operationsStringsList.push_back(operationsStrings);

			if (segmentEnd == variableEnd)
				break;
			segmentStart = segmentEnd + 1;
		}

		if (!Leaf::_ParseStringListOperationsRecursive(
				context,
				operationsStringsList,
				0,
				data::StringListOperations(),
				operationsList)) {
			return StringList();
		}
	} else
		operationsList.push_back(data::StringListOperations());
	size_t operationsCount = operationsList.size();

	// Iterate through the variable list and for each perform all subscript
	// and string operations.
	StringList resultValue;
	for (size_t variableIndex = 0; variableIndex < variableCount;
		 variableIndex++) {
		StringList originalVariableValue =
			context.LookupVariable(variableNames.ElementAt(variableIndex));

		// Iterate through the range subscripts. For each perform all string
		// operations.
		for (size_t subscriptsIndex = 0; subscriptsIndex < subscriptsCount;
			 subscriptsIndex++) {
			std::pair<size_t, size_t> range = subscripts.at(subscriptsIndex);
			size_t maxSize =
				range.second > range.first ? range.second - range.first : 0;

			StringList variableValue = originalVariableValue.SubList(
				range.first,
				std::numeric_limits<size_t>::max());

			// Iterate through the operations.
			for (size_t operationsIndex = 0; operationsIndex < operationsCount;
				 operationsIndex++) {
				data::StringListOperations operations =
					operationsList.at(operationsIndex);
				resultValue.Append(operations.Apply(variableValue,
													maxSize,
													context.GetBehavior()));
			}
		}
	}

	return resultValue;
}

/*static*/ bool
Leaf::_ParseSubscripts(const char* start,
					   const char* end,
					   size_t& _firstIndex,
					   size_t& _endIndex)
{
	// TODO: Since Jam doesn't do much sanity checking of what it parses, its
	// behavior is weird for invalid input. We don't copy all of that behavior
	// yet.

	// Jam subscripts are 1-based and the end subscript is inclusive. We convert
	// to 0-based indices and an exclusive end index.

	// first subscript
	char* numberEnd;
	long firstIndex = strtol(start, &numberEnd, 10);
	if (numberEnd == start || numberEnd > end)
		return false;

	_firstIndex = firstIndex > 0 ? firstIndex - 1 : 0;

	// last subscript (optional)
	if (numberEnd == end || *numberEnd != '-') {
		_endIndex = _firstIndex + 1;
		return true;
	}

	const char* lastStart = numberEnd + 1;
	if (lastStart == end) {
		_endIndex = std::numeric_limits<size_t>::max();
		return true;
	}

	long endIndex = strtol(lastStart, &numberEnd, 10);
	if (numberEnd == lastStart || numberEnd > end)
		return false;

	// Emulate Jam behavior: Since jam computes a size from the second subscript
	// before checking the validity of the first subscript, we are off by as
	// much as we adjusted the first subscript, if we compute the size
	// afterwards. Compensate for that.
	if (firstIndex < 1)
		endIndex += 1 - firstIndex;

	_endIndex = endIndex >= 0 ? endIndex : 0;
	return true;
}

/*static*/ bool
Leaf::_ParseStringListOperationsRecursive(
	EvaluationContext& context,
	const std::vector<StringList>& operationsStringsList,
	size_t operationsStringsListIndex,
	data::StringListOperations operations,
	std::vector<data::StringListOperations>& _operationsList)
{
	// Note: operationsStrings.ElementAt() returns a new String object, but due
	// to copy-on-write it refers to the same underlying buffer. So the
	// parameters of the operations we create refer to valid string parts.
	for (;;) {
		const StringList& operationsStrings =
			operationsStringsList.at(operationsStringsListIndex);

		size_t count = operationsStrings.Size();

		// common case: only one operations string
		if (count == 1) {
			String string = operationsStrings.ElementAt(0);
			operations.Parse(string.ToCString(),
							 string.ToCString() + string.Length());

			// no need to recurse, just iterate
			if (++operationsStringsListIndex == operationsStringsList.size()) {
				_operationsList.push_back(operations);
				return true;
			}
			continue;
		}

		// more than one operation (or none) -- parse each and recurse
		for (size_t i = 0; i < count; i++) {
			String string = operationsStrings.ElementAt(i);
			data::StringListOperations newOperations = operations;
			newOperations.Parse(string.ToCString(),
								string.ToCString() + string.Length());

			if (operationsStringsListIndex + 1
				== operationsStringsList.size()) {
				_operationsList.push_back(newOperations);
			} else {
				_ParseStringListOperationsRecursive(context,
													operationsStringsList,
													operationsStringsListIndex
														+ 1,
													newOperations,
													_operationsList);
			}
		}
		return true;
	}
}

} // namespace code
} // namespace ham
