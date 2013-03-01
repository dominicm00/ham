/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_OPTION_ITERATOR_H
#define HAM_UTIL_OPTION_ITERATOR_H


#include <map>
#include <string>


namespace ham {
namespace util {


class OptionSpecification {
public:
			struct Option {
				int			fShortOption;
				std::string	fLongOption;
				bool		fHasArgument;
			};

public:
			OptionSpecification& Add(int shortOption,
									const char* longOption = NULL,
									bool hasArgument = false);
									// shortOption is the the short option char,
									// if there is one,  or a unique int >= 256,
									// if there's only a long option. The value
									// specified here is returned by
									// OptionIterator::Next().

			const Option*		ShortOption(int option) const;
			const Option*		LongOption(const std::string option) const;

private:
			std::map<int, Option> fShortOptions;
			std::map<std::string, Option> fLongOptions;
};


class OptionIterator {
public:
								OptionIterator(int argumentCount,
									const char* const* arguments,
									const OptionSpecification& options);

			bool				HasNext() const
									{ return fStatus == MORE_OPTIONS; }
			int					Next(std::string& argument);

			bool				ErrorOccurred() const
									{ return fStatus == ERROR; }
			int					Index() const
									{ return fIndex; }

private:
			typedef OptionSpecification::Option Option;

			enum Status {
				MORE_OPTIONS,
				NO_MORE_OPTIONS,
				ERROR
			};

private:
			void				_FindNext()
									{ fStatus = _FindNextInternal(); }
			Status				_FindNextInternal();

private:
			OptionSpecification	fOptions;
			const char* const*	fArguments;
			int					fArgumentCount;
			int					fIndex;
			const Option*		fCurrentOption;
			const char*			fOptionArgument;
			const char*			fRemainingShortOptions;
			Status				fStatus;
};


}	// namespace util
}	// namespace ham


#endif	// HAM_UTIL_OPTION_ITERATOR_H
