/*
 * Copyright 2010-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_H
#define HAM_DATA_STRING_H


#include <string.h>

#include <list>
#include <ostream>
#include <string>

#include "data/StringPart.h"
#include "util/Referenceable.h"


namespace ham {
namespace data {


class StringList;


class String {
public:
								String();
								String(const char* string);
								String(const char* string, size_t maxLength);
	explicit					String(const StringPart& string);
								String(const String& other);
								~String();

			const char*			ToCString() const
									{ return fBuffer->fString; }
			std::string			ToStlString() const
									{ return std::string(fBuffer->fString); }
			size_t				Length() const
									{ return fBuffer->fLength; }
			bool				IsEmpty() const
									{ return Length() == 0; }

			String&				ToUpper();
			String&				ToLower();

	inline	int					CompareWith(const String& other) const;

			bool				operator==(const String& other) const
									{ return CompareWith(other) == 0; }
			bool				operator!=(const String& other) const
									{ return !(*this == other); }
			bool				operator<(const String& other) const
									{ return CompareWith(other) < 0; }
			bool				operator>(const String& other) const
									{ return other < *this; }
			bool				operator<=(const String& other) const
									{ return !(*this > other); }
			bool				operator>=(const String& other) const
									{ return !(*this < other); }

			String&				operator=(const String& other);
			String&				operator=(const StringPart& string)
									{ return *this = String(string); }
			String				operator+(const String& other) const;
	inline	String				operator+(const StringPart& other) const;
	friend	String				operator+(const StringPart& string1,
									const String& string2);
	friend	String				operator+(const StringPart& string1,
									const StringPart& string2);

	inline	operator			StringPart() const;

private:
			friend class StringList;

			struct Buffer {
				static Buffer* Create(size_t length)
				{
					void* memory = malloc(sizeof(Buffer) + length);
					if (memory == NULL)
						throw std::bad_alloc();
					return new(memory) Buffer(length);
				}

				void Acquire()
				{
					util::increment_reference_count(fReferenceCount);
				}

				void Release()
				{
					if (util::decrement_reference_count(fReferenceCount) == 1)
						free(this);
				}

			private:
				Buffer(size_t length)
					:
					fReferenceCount(1),
					fLength(length)
				{
					fString[length] = '\0';
				}

				~Buffer()
				{
				}

			public:
				int32_t	fReferenceCount;
				size_t	fLength;
				char	fString[1];

				static Buffer sEmptyBuffer;
			};

private:
								String(Buffer* buffer);

	static	String				_Concatenate(const char* string1,
									size_t length1, const char* string2,
									size_t length2);

	inline	Buffer*				_CreateBuffer(const char* string,
									size_t length);
	inline	void				_CopyOnWriteBuffer();

private:
			Buffer*				fBuffer;
};


inline int
String::CompareWith(const String& other) const
{
	return strcmp(ToCString(), other.ToCString());
}


inline String
String::operator+(const StringPart& other) const
{
	if (other.IsEmpty())
		return *this;
	return _Concatenate(ToCString(), Length(), other.Start(), other.Length());
}


inline String
operator+(const StringPart& string1, const String& string2)
{
	if (string1.IsEmpty())
		return string2;
	return String::_Concatenate(string1.Start(), string1.Length(),
		string2.ToCString(), string2.Length());
}


inline String
operator+(const StringPart& string1, const StringPart& string2)
{
	return String::_Concatenate(string1.Start(), string1.Length(),
		string2.Start(), string2.Length());
}


inline
String::operator StringPart() const
{
	return StringPart(ToCString(), Length());
}


}	// namespace data


using data::String;


}	// namespace ham


inline
std::ostream& operator<<(std::ostream& stream, const ham::data::String& string)
{
	return stream << string.ToStlString();
}


#endif	// HAM_DATA_STRING_H
