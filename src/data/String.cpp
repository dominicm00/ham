/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/String.h"


namespace ham {
namespace data {


String::Buffer String::Buffer::sEmptyBuffer(0);


String::String()
	:
	fBuffer(&Buffer::sEmptyBuffer)
{
	fBuffer->Acquire();
}


String::String(const char* string)
	:
	fBuffer(_CreateBuffer(string, strlen(string)))
{
}


String::String(const char* string, size_t maxLength)
	:
	fBuffer(_CreateBuffer(string, strnlen(string, maxLength)))
{
}


String::String(const String& other)
	:
	fBuffer(other.fBuffer)
{
	fBuffer->Acquire();
}


String::~String()
{
	fBuffer->Release();
}


String&
String::operator=(const String& other)
{
	if (this != &other) {
		fBuffer->Release();
		fBuffer = other.fBuffer;
		fBuffer->Acquire();
	}

	return *this;
}


String
String::operator+(const String& other) const
{
	size_t otherLength = other.Length();
	if (otherLength == 0)
		return *this;

	size_t length = Length();
	if (length == 0)
		return other;

	Buffer* buffer = Buffer::Create(length + otherLength);
	memcpy(buffer->fString, ToCString(), length);
	memcpy(buffer->fString + length, other.fBuffer->fString, otherLength);

	return String(buffer);
}

String::String(String::Buffer* buffer)
	:
	fBuffer(buffer)
{
}


/*static*/ String
String::_Concatenate(const char* string1, size_t length1, const char* string2,
	size_t length2)
{
	if (length1 == 0 && length2 == 0)
		return String();

	Buffer* buffer = Buffer::Create(length1 + length2);
	memcpy(buffer->fString, string1, length1);
	memcpy(buffer->fString + length1, string2, length2);

	return String(buffer);
}


String::Buffer*
String::_CreateBuffer(const char* string, size_t length)
{
	if (length == 0) {
		Buffer::sEmptyBuffer.Acquire();
		return &Buffer::sEmptyBuffer;
	}

	Buffer* buffer = Buffer::Create(length);
	memcpy(buffer->fString, string, length);
	return buffer;
}


}	// namespace data
}	// namespace ham
