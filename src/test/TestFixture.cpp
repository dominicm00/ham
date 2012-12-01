/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/TestFixture.h"

#include <sstream>


namespace test {


data::StringList
TestFixture::MakeStringList(const char* element1, const char* element2,
	const char* element3, const char* element4, const char* element5,
	const char* element6, const char* element7, const char* element8,
	const char* element9, const char* element10)
{
	const char* const elements[] = {
		element1, element2, element3, element4, element5, element6, element7,
		element8, element9, element10
	};

	data::StringList list;
	for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
		if (elements[i] == NULL)
			break;
		list.push_back(data::String(elements[i]));
	}

	return list;
}


#define DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(type)	\
	template<>												\
	std::string												\
	TestFixture::ValueToString<type>(const type& value)		\
	{														\
		std::ostringstream stream;							\
		stream << value;									\
		return stream.str();								\
	}


DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(char)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(short)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(int)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(long long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned char)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned short)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned int)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(unsigned long long)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(bool)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(float)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(double)
DEFINE_SSTEAM_VALUE_TO_STRING_SPECIALIZATION(long double)


template<>
std::string
TestFixture::ValueToString<std::string>(const std::string& value)
{
	return value;
}


template<>
std::string
TestFixture::ValueToString<std::list<std::string> >(
	const std::list<std::string>& value)
{
	std::ostringstream stream;
	stream << "{";

	for (std::list<std::string>::const_iterator it = value.begin();
		it != value.end(); ++it) {
		stream << ' ' << '\'' << *it << '\'';
	}

	stream << " }";
	return stream.str();
}


} // namespace test
