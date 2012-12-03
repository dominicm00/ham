/*
 * Copyright 2010-2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_H
#define HAM_DATA_STRING_H


#include <string.h>

#include <list>
#include <string>


namespace ham {
namespace data {


class String {
public:
								String();
								String(const char* string);
								String(const char* string, size_t maxLength);
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
			String				operator+(const String& other) const;

private:
			struct Buffer {
				Buffer(size_t length)
					:
					fReferenceCount(1),
					fString(new char[length + 1]),
					fLength(length)
				{
					fString[length] = '\0';
				}

				~Buffer()
				{
					delete[] fString;
				}

				void Acquire()
				{
					__sync_fetch_and_add(&fReferenceCount, 1);
				}

				void Release()
				{
					if (__sync_fetch_and_sub(&fReferenceCount, 1) == 1)
						delete this;
				}

			public:
				int		fReferenceCount;
				char*	fString;
				size_t	fLength;
			};

private:
								String(Buffer* buffer);

	inline	Buffer*				_CreateBuffer(const char* string,
									size_t length);

private:
			Buffer*				fBuffer;

	static	Buffer				sEmptyBuffer;
};


inline int
String::CompareWith(const String& other) const
{
	return strcmp(ToCString(), other.ToCString());
}


}	// namespace data


using data::String;


}	// namespace ham


#include <ostream>

//template<typename Stream>
//Stream& operator<<(Stream& stream, const ham::data::String& string)
static inline std::ostream& operator<<(std::ostream& stream, const ham::data::String& string)
{
	return stream << string.ToStlString();
}


#endif	// HAM_DATA_STRING_H
