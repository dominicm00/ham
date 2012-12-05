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


class StringList;


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
					__sync_fetch_and_add(&fReferenceCount, 1);
				}

				void Release()
				{
					if (__sync_fetch_and_sub(&fReferenceCount, 1) == 1)
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
				int		fReferenceCount;
				size_t	fLength;
				char	fString[1];

				static Buffer sEmptyBuffer;
			};

private:
								String(Buffer* buffer);

	inline	Buffer*				_CreateBuffer(const char* string,
									size_t length);

private:
			Buffer*				fBuffer;
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
