/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_STRING_LIST_HPP
#define HAM_DATA_STRING_LIST_HPP

#include "data/String.hpp"
#include "util/Referenceable.hpp"

#include <new>
#include <stdlib.h>
#include <vector>

namespace ham
{
namespace data
{

class StringList;
typedef std::vector<StringList> StringListList;

// TODO: Should be replaced with ranges and std::string
class StringList
{
  public:
	class Iterator;

  public:
	StringList();
	StringList(size_t elementCount);
	StringList(const String& string);
	StringList(const StringList* other);
	StringList(const StringList& other, size_t startIndex, size_t endIndex);
	StringList(const StringList& other);
	~StringList();

	inline Iterator GetIterator() const;

	size_t Size() const { return fSize; }
	bool IsEmpty() const { return fSize == 0; }

	bool IsTrue() const;
	// any element is non-empty

	String Head() const { return ElementAt(0); }
	inline String ElementAt(size_t index) const;
	inline void SetElementAt(size_t index, const String& value);

	inline StringList SubList(size_t startIndex, size_t endIndex) const;

	bool Contains(const String& string) const;

	int CompareWith(
		const StringList& other,
		bool ignoreTrailingEmptyStrings = false
	) const;

	StringList& Append(const String& string);
	StringList& Append(const StringList& list);
	void Clear() { *this = kFalse; }

	String Join() const;
	String Join(const StringPart& separator) const;

	static const StringList& True() { return kTrue; }
	static const StringList& False() { return kFalse; }

	static StringList Multiply(const StringListList& listList);

	bool operator==(const StringList& other) const;
	bool operator!=(const StringList& other) const { return !(*this == other); }
	bool operator<(const StringList& other) const
	{
		return CompareWith(other) < 0;
	}
	bool operator>(const StringList& other) const { return other < *this; }
	bool operator<=(const StringList& other) const { return !(*this > other); }
	bool operator>=(const StringList& other) const { return !(*this < other); }

	StringList& operator=(const StringList& other);

  private:
	class Data
	{
	  public:
		static Data* Create(size_t capacity)
		{
			void* memory = malloc(sizeof(Data) + sizeof(String) * capacity);
			if (memory == nullptr)
				throw std::bad_alloc();
			return new (memory) Data(capacity);
		}

		void Acquire() { util::increment_reference_count(fReferenceCount); }

		void Release()
		{
			if (util::decrement_reference_count(fReferenceCount) == 1) {
				for (size_t i = 0; i < fSize; i++)
					DestroyElement(i);
				free(this);
			}
		}

		void ConstructElement(size_t index, const String& string)
		{
			new (&fElements[index]) String(string);
		}

		void DestroyElement(size_t index) { fElements[index].~String(); }

	  private:
		Data(size_t capacity)
			: fReferenceCount(1),
			  fSize(0),
			  fCapacity(capacity)
		{
		}

		~Data() {}

	  public:
		int32_t fReferenceCount;
		size_t fSize;
		size_t fCapacity;
		String fElements[1];

		static Data sEmptyData;
	};

  private:
	inline Data* _CreateData(size_t size);
	void _Detach(size_t newSize);
	static size_t _CapacityForSize(size_t size);

  private:
	Data* fData;
	size_t fOffset;
	size_t fSize;

	static const StringList kTrue;
	static const StringList kFalse;
};

class StringList::Iterator
{
  public:
	Iterator()
		: fList(nullptr),
		  fNextIndex(0)
	{
	}

	Iterator(const StringList& list)
		: fList(&list),
		  fNextIndex(0)
	{
	}

	bool HasNext() const
	{
		return fList != nullptr && fNextIndex < fList->fSize;
	}

	String Next()
	{
		if (!HasNext())
			return String();
		return fList->ElementAt(fNextIndex++);
	}

	bool operator==(const Iterator& other) const
	{
		return fList == other.fList && fNextIndex == other.fNextIndex;
	}

	bool operator!=(const Iterator& other) const { return !(*this == other); }

  private:
	const StringList* fList;
	size_t fNextIndex;
};

inline StringList::Iterator
StringList::GetIterator() const
{
	return Iterator(*this);
}

inline String
StringList::ElementAt(size_t index) const
{
	return index < fSize ? fData->fElements[fOffset + index] : String();
}

inline void
StringList::SetElementAt(size_t index, const String& value)
{
	if (index < fSize) {
		_Detach(fSize);
		fData->fElements[index] = value;
	}
}

inline StringList
StringList::SubList(size_t startIndex, size_t endIndex) const
{
	return StringList(*this, startIndex, endIndex);
}

} // namespace data

using data::StringList;
using data::StringListList;

} // namespace ham

inline std::ostream&
operator<<(std::ostream& stream, const ham::data::StringList& stringList)
{
	stream << "{";

	for (ham::data::StringList::Iterator it = stringList.GetIterator();
		 it.HasNext();) {
		if (it != stringList.GetIterator())
			stream << ", ";
		stream << it.Next();
	}

	return stream << "}";
}

#endif // HAM_DATA_STRING_LIST_HPP
