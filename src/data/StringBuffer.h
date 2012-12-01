/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_BUFFER_H
#define HAM_DATA_STRING_BUFFER_H


#include <string>


namespace ham {
namespace data {


class StringBuffer {
public:
	inline						StringBuffer();
	inline						StringBuffer(const StringBuffer& other);
	inline						~StringBuffer();

	inline	const char*			Data() const;
	inline	size_t				Length() const;

	inline	StringBuffer&		operator=(const StringBuffer& other);

	inline	StringBuffer&		operator+=(char c);
	inline	StringBuffer&		operator+=(const StringBuffer& other);

	inline	bool				operator==(const StringBuffer& other) const;
	inline	bool				operator!=(const StringBuffer& other) const;

	inline	char				operator[](int index) const;

	inline						operator std::string() const;
	inline						operator const char*() const;

	template<typename Output>
	friend inline Output&		operator<<(Output& out,
									const StringBuffer& buffer);

private:
			enum {
				MIN_BUFFER_SIZE = 256
			};

private:
			std::string			fData;
};




StringBuffer::StringBuffer()
{
}


StringBuffer::StringBuffer(const StringBuffer& other)
	:
	fData(other.fData)
{
}


StringBuffer::~StringBuffer()
{
}


const char*
StringBuffer::Data() const
{
	return fData.c_str();
}


size_t
StringBuffer::Length() const
{
	return fData.length();
}


StringBuffer&
StringBuffer::operator=(const StringBuffer& other)
{
	fData = other.fData;
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
	size_t newSize = fData.size() + other.fData.size();
	if (newSize > fData.capacity())
		fData.reserve(std::max(newSize * 2, (size_t)MIN_BUFFER_SIZE));

	fData += other.fData;
	return *this;
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


StringBuffer::operator std::string() const
{
	return std::string(Data());
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


}	// namespace data
}	// namespace ham


#endif	// HAM_DATA_STRING_BUFFER_H
