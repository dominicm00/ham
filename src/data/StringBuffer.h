/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_BUFFER_H
#define HAM_DATA_STRING_BUFFER_H


#include <string>


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

	inline						operator std::string() const;

	template<typename Output>
	friend inline Output&		operator<<(Output& out,
									const StringBuffer& buffer);

private:
			std::string			fData;

	static	const size_t		kMinBufferSize = 256;
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
		fData.reserve(std::max(newSize * 2, kMinBufferSize));

	fData += c;
	return *this;
}


StringBuffer&
StringBuffer::operator+=(const StringBuffer& other)
{
	size_t newSize = fData.size() + other.fData.size();
	if (newSize > fData.capacity())
		fData.reserve(std::max(newSize * 2, kMinBufferSize));

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


StringBuffer::operator std::string() const
{
	return std::string(Data());
}


template<typename Output>
inline Output&
operator<<(Output& out, const StringBuffer& buffer)
{
	return out << buffer.fData;
}


} // namespace data


#endif	// HAM_DATA_STRING_BUFFER_H
