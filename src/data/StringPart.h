/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_PART_H
#define HAM_DATA_STRING_PART_H

#include <string.h>

#include <algorithm>
#include <string>

namespace ham
{
namespace data
{

class StringPart
{
  public:
	StringPart() { Unset(); }
	explicit StringPart(const char* string) { SetTo(string, strlen(string)); }
	StringPart(const char* start, const char* end) { SetTo(start, end); }
	StringPart(const char* start, size_t length) { SetTo(start, length); }
	StringPart(const StringPart& other) { *this = other; }

	void SetTo(const char* string);
	void SetTo(const char* start, const char* end);
	void SetTo(const char* start, size_t length);
	void Unset();

	std::string ToStlString() const;

	const char* Start() const { return fStart; }
	const char* End() const { return fEnd; }
	size_t Length() const { return fEnd - fStart; }
	bool IsEmpty() const { return fStart == fEnd; }

	int CompareWith(const StringPart& other) const;

	bool operator==(const StringPart& other) const
	{
		return CompareWith(other) == 0;
	}
	bool operator!=(const StringPart& other) const { return !(*this == other); }
	bool operator<(const StringPart& other) const
	{
		return CompareWith(other) < 0;
	}
	bool operator>(const StringPart& other) const { return other < *this; }
	bool operator<=(const StringPart& other) const { return !(*this > other); }
	bool operator>=(const StringPart& other) const { return !(*this < other); }

	StringPart& operator=(const StringPart& other);

  private:
	const char* fStart;
	const char* fEnd;
};

inline void
StringPart::SetTo(const char* string)
{
	SetTo(string, strlen(string));
}

inline void
StringPart::SetTo(const char* start, const char* end)
{
	fStart = start;
	fEnd = end > start ? end : start;
}

inline void
StringPart::SetTo(const char* start, size_t length)
{
	fStart = start;
	fEnd = start + length;
}

inline void
StringPart::Unset()
{
	fStart = NULL;
	fEnd = NULL;
}

inline std::string
StringPart::ToStlString() const
{
	return std::string(fStart, fEnd - fStart);
}

int inline StringPart::CompareWith(const StringPart& other) const
{
	size_t length = Length();
	size_t otherLength = other.Length();
	size_t toCompare = std::min(length, otherLength);
	int compare = toCompare > 0 ? strncmp(fStart, other.fStart, toCompare) : 0;
	return compare != 0 ? compare : (int)length - (int)otherLength;
}

inline StringPart&
StringPart::operator=(const StringPart& other)
{
	fStart = other.fStart;
	fEnd = other.fEnd;
	return *this;
}

} // namespace data

using data::StringPart;

} // namespace ham

#endif // HAM_DATA_STRING_PART_H
