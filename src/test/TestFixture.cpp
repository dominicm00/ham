/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "test/TestFixture.h"

#include <sstream>
#include <vector>


namespace ham {
namespace test {


template<typename Container>
static std::string
value_container_to_string(const Container& value)
{
	std::ostringstream stream;
	stream << "{";

	for (typename Container::const_iterator it = value.begin();
			it != value.end(); ++it) {
		stream << ' ' << TestFixture::ValueToString(*it);
	}

	stream << " }";
	return stream.str();
}


/*static*/ data::StringList
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
		list.Append(data::String(elements[i]));
	}

	return list;
}


/*static*/ data::StringList
TestFixture::MakeStringList(const std::vector<std::string>& testList)
{
	StringList list;
	for (std::vector<std::string>::const_iterator it = testList.begin();
			it != testList.end(); ++it) {
		list.Append(it->c_str());
	}

	return list;
}

/*static*/ data::StringListList
TestFixture::MakeStringListList(
	const std::vector<std::vector<std::string> >& testListList)
{
	StringListList listList;
	for (std::vector<std::vector<std::string> >::const_iterator it
			= testListList.begin(); it != testListList.end(); ++it) {
		listList.push_back(MakeStringList(*it));
	}

	return listList;
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
/*static*/ std::string
TestFixture::ValueToString<const char*>(const char* const& value)
{
	return std::string("'") + value + "'";
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::string>(const std::string& value)
{
	return ValueToString(value.c_str());
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::list<std::string> >(
	const std::list<std::string>& value)
{
	return value_container_to_string(value);
}


template<>
/*static*/ std::string
TestFixture::ValueToString<std::vector<std::string> >(
	const std::vector<std::string>& value)
{
	return value_container_to_string(value);
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::StringPart>(const data::StringPart& value)
{
	return TestFixture::ValueToString(value.ToStlString());
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::String>(const data::String& value)
{
	return TestFixture::ValueToString(value.ToCString());
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::StringList>(const data::StringList& value)
{
	std::ostringstream stream;
	stream << "{";

	for (StringList::Iterator it = value.GetIterator(); it.HasNext();)
		stream << ' ' << ValueToString(it.Next());

	stream << " }";
	return stream.str();
}


template<>
/*static*/ std::string
TestFixture::ValueToString<data::StringListList>(
	const data::StringListList& value)
{
	return value_container_to_string(value);
}


} // namespace test
} // namespace ham
