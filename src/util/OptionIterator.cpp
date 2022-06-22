/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "util/OptionIterator.hpp"

namespace ham
{
namespace util
{

// #pragma mark - OptionSpecification

OptionSpecification&
OptionSpecification::Add(int shortOption,
						 const char* longOption,
						 bool hasArgument)
{
	Option option = {shortOption, longOption, hasArgument};
	fShortOptions[shortOption] = option;
	if (longOption != nullptr)
		fLongOptions[longOption] = option;
	return *this;
}

const OptionSpecification::Option*
OptionSpecification::ShortOption(int option) const
{
	std::map<int, Option>::const_iterator it = fShortOptions.find(option);
	return it != fShortOptions.end() ? &it->second : nullptr;
}

const OptionSpecification::Option*
OptionSpecification::LongOption(const std::string option) const
{
	std::map<std::string, Option>::const_iterator it =
		fLongOptions.find(option);
	return it != fLongOptions.end() ? &it->second : nullptr;
}

// #pragma mark - OptionIterator

OptionIterator::OptionIterator(int argumentCount,
							   const char* const* arguments,
							   const OptionSpecification& options)
	: fOptions(options),
	  fArguments(arguments),
	  fArgumentCount(argumentCount),
	  fIndex(1),
	  fCurrentOption(nullptr),
	  fOptionArgument(nullptr),
	  fRemainingShortOptions(nullptr),
	  fStatus(MORE_OPTIONS)
{
	_FindNext();
}

int
OptionIterator::Next(std::string& argument)
{
	if (!HasNext())
		return '\0';

	char option = fCurrentOption->fShortOption;
	argument = fOptionArgument != nullptr ? fOptionArgument : "";

	_FindNext();
	return option;
}

OptionIterator::Status
OptionIterator::_FindNextInternal()
{
	if (fStatus != MORE_OPTIONS)
		return fStatus;

	if (fIndex >= fArgumentCount)
		return NO_MORE_OPTIONS;

	if (fRemainingShortOptions == nullptr) {
		const char* argument = fArguments[fIndex];
		if (argument[0] != '-')
			return NO_MORE_OPTIONS;

		fIndex++;

		if (argument[1] == '\0') {
			// "-" argument
			return ERROR;
		}

		if (argument[1] == '-') {
			// long option
			fCurrentOption = fOptions.LongOption(argument);
			if (fCurrentOption == nullptr) {
				// unknown long option
				return ERROR;
			}

			if (fCurrentOption->fHasArgument) {
				if (fIndex >= fArgumentCount) {
					// missing option argument
					return ERROR;
				}

				fOptionArgument = fArguments[fIndex++];
			} else
				fOptionArgument = nullptr;

			return MORE_OPTIONS;
		}

		// short option(s) -- fall through
		fRemainingShortOptions = argument + 1;
	}

	// short option
	fCurrentOption = fOptions.ShortOption(fRemainingShortOptions[0]);

	fRemainingShortOptions++;
	if (fRemainingShortOptions[0] == '\0')
		fRemainingShortOptions = nullptr;

	if (fCurrentOption == nullptr) {
		// unknown short option
		return ERROR;
	}

	if (fCurrentOption->fHasArgument) {
		if (fRemainingShortOptions != nullptr) {
			// no space between option and argument
			fOptionArgument = fRemainingShortOptions;
			fRemainingShortOptions = nullptr;
		} else {
			// option argument is separate argument
			if (fIndex >= fArgumentCount) {
				// missing option argument
				return ERROR;
			}

			fOptionArgument = fArguments[fIndex++];
		}
	} else
		fOptionArgument = nullptr;

	return MORE_OPTIONS;
}

} // namespace util
} // namespace ham
