/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_BUFFER_HPP
#define HAM_DATA_STRING_BUFFER_HPP

#include <string>

#include "data/String.hpp"

namespace ham
{
namespace data
{

// TODO: This should be replaced with std::string.
class StringBuffer
{
  public:
	inline StringBuffer();
	inline StringBuffer(const StringBuffer& other);
	inline ~StringBuffer();

	inline const char* Data() const;
	inline char* Data();
	inline size_t Length() const;

	inline StringBuffer& Append(const char* string, size_t length);

	inline StringBuffer& operator=(const StringBuffer& other);
	inline StringBuffer& operator=(const StringPart& string);
	inline StringBuffer& operator=(const String& string);

	inline StringBuffer& operator+=(char c);
	inline StringBuffer& operator+=(const StringBuffer& other);
	inline StringBuffer& operator+=(const StringPart& string);
	inline StringBuffer& operator+=(const String& string);

	inline bool operator==(const StringBuffer& other) const;
	inline bool operator!=(const StringBuffer& other) const;

	inline char operator[](int index) const;

	//	inline						operator std::string() const;
	inline operator String() const;
	inline operator const char*() const;

	template<typename Output>
	friend inline Output& operator<<(Output& out, const StringBuffer& buffer);

  private:
	enum { MIN_BUFFER_SIZE = 256 };

  private:
	std::string fData;
};

StringBuffer::StringBuffer() {}

StringBuffer::StringBuffer(const StringBuffer& other)
	: fData(other.fData)
{
}

StringBuffer::~StringBuffer() {}

const char*
StringBuffer::Data() const
{
	return fData.c_str();
}

char*
StringBuffer::Data()
{
	return &fData[0];
}

size_t
StringBuffer::Length() const
{
	return fData.length();
}

StringBuffer&
StringBuffer::Append(const char* string, size_t length)
{
	if (length > 0) {
		size_t newSize = fData.size() + length;
		if (newSize > fData.capacity())
			fData.reserve(std::max(newSize * 2, (size_t)MIN_BUFFER_SIZE));

		fData.append(string, string + length);
	}

	return *this;
}

StringBuffer&
StringBuffer::operator=(const StringBuffer& other)
{
	fData = other.fData;
	return *this;
}

StringBuffer&
StringBuffer::operator=(const StringPart& string)
{
	fData = string.ToStlString();
	return *this;
}

StringBuffer&
StringBuffer::operator=(const String& string)
{
	fData = string.ToStlString();
	return *this;
}

StringBuffer&
StringBuffer::operator+=(char c)
{
	size_t newSize = fData.size() + 1;
	if (newSize > fData.capacity())
		fData.reserve(std::max(newSize * 2, (size_t)MIN_BUFFER_SIZE));

	fData += c;
	return *this;
}

StringBuffer&
StringBuffer::operator+=(const StringBuffer& other)
{
	return Append(other.Data(), other.Length());
}

StringBuffer&
StringBuffer::operator+=(const StringPart& string)
{
	return Append(string.Start(), string.Length());
}

StringBuffer&
StringBuffer::operator+=(const String& string)
{
	return Append(string.ToCString(), string.Length());
}

bool
StringBuffer::operator==(const StringBuffer& other) const
{
	return fData == other.fData;
}

bool
StringBuffer::operator!=(const StringBuffer& other) const
{
	return fData != other.fData;
}

char
StringBuffer::operator[](int index) const
{
	return fData[index];
}

// StringBuffer::operator std::string() const
//{
//	return std::string(Data());
// }

StringBuffer::operator String() const
{
	return String(Data());
}

StringBuffer::operator const char*() const
{
	return Data();
}

template<typename Output>
inline Output&
operator<<(Output& out, const StringBuffer& buffer)
{
	return out << buffer.fData;
}

} // namespace data

using data::StringBuffer;

} // namespace ham

#endif // HAM_DATA_STRING_BUFFER_HPP
