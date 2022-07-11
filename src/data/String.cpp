/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "data/String.hpp"

namespace ham::data
{

String::Buffer String::Buffer::sEmptyBuffer(0);

String::String()
	: fBuffer(&Buffer::sEmptyBuffer)
{
	fBuffer->Acquire();
}

String::String(const char* string)
	: fBuffer(_CreateBuffer(string, strlen(string)))
{
}

String::String(const char* string, size_t maxLength)
	: fBuffer(_CreateBuffer(string, strnlen(string, maxLength)))
{
}

String::String(const StringPart& string)
	: fBuffer(_CreateBuffer(string.Start(), string.Length()))
{
}

String::String(const String& other)
	: fBuffer(other.fBuffer)
{
	fBuffer->Acquire();
}

String::~String() { fBuffer->Release(); }

String
String::SubString(size_t startOffset, size_t endOffset) const
{
	if (endOffset >= Length()) {
		if (startOffset == 0)
			return *this;
		endOffset = Length();
	}

	if (startOffset >= Length() || startOffset >= endOffset)
		return String();

	return String(
		_CreateBuffer(ToCString() + startOffset, endOffset - startOffset)
	);
}

String&
String::ToUpper()
{
	_CopyOnWriteBuffer();

	char* string = fBuffer->fString;
	std::transform(string, string + fBuffer->fLength, string, ::toupper);

	return *this;
}

String&
String::ToLower()
{
	_CopyOnWriteBuffer();

	char* string = fBuffer->fString;
	std::transform(string, string + fBuffer->fLength, string, ::tolower);

	return *this;
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
	: fBuffer(buffer)
{
}

/*static*/ String
String::_Concatenate(
	const char* string1,
	size_t length1,
	const char* string2,
	size_t length2
)
{
	if (length1 == 0 && length2 == 0)
		return String();

	Buffer* buffer = Buffer::Create(length1 + length2);
	memcpy(buffer->fString, string1, length1);
	memcpy(buffer->fString + length1, string2, length2);

	return String(buffer);
}

/*static*/ String::Buffer*
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

void
String::_CopyOnWriteBuffer()
{
	if (fBuffer->fReferenceCount == 1)
		return;

	Buffer* buffer = _CreateBuffer(fBuffer->fString, fBuffer->fLength);
	fBuffer->Release();
	fBuffer = buffer;
}

} // namespace ham::data
