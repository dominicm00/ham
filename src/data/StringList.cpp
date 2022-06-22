/*
 * Copyright 2012-2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "StringList.hpp"

#include <algorithm>
#include <vector>

namespace ham
{
namespace data
{

StringList::Data StringList::Data::sEmptyData(0);
const StringList StringList::kTrue("1");
const StringList StringList::kFalse;

static const size_t kMinCapacity = 8;

StringList::StringList()
	: fData(&Data::sEmptyData),
	  fOffset(0),
	  fSize(0)
{
	fData->Acquire();
}

StringList::StringList(size_t elementCount)
	: fData(_CreateData(elementCount)),
	  fOffset(0),
	  fSize(elementCount)
{
	for (size_t i = 0; i < elementCount; i++)
		fData->ConstructElement(i, String());
	fData->fSize = elementCount;
}

StringList::StringList(const String& string)
	: fData(_CreateData(1)),
	  fOffset(0),
	  fSize(1)
{
	fData->ConstructElement(0, string);
	fData->fSize = 1;
}

StringList::StringList(const StringList* other)
{
	if (other != nullptr) {
		fData = other->fData;
		fOffset = other->fOffset;
		fSize = other->fSize;
	} else {
		fData = &Data::sEmptyData;
		fOffset = 0;
		fSize = 0;
	}

	fData->Acquire();
}

StringList::StringList(const StringList& other,
					   size_t startIndex,
					   size_t endIndex)
{
	if (startIndex < other.fSize && startIndex < endIndex) {
		fData = other.fData;
		fOffset = other.fOffset + startIndex;
		fSize = std::min(endIndex, other.fSize) - startIndex;
	} else {
		fData = &Data::sEmptyData;
		fOffset = 0;
		fSize = 0;
	}

	fData->Acquire();
}

StringList::StringList(const StringList& other)
	: fData(other.fData),
	  fOffset(other.fOffset),
	  fSize(other.fSize)
{
	fData->Acquire();
}

StringList::~StringList()
{
	fData->Release();
}

bool
StringList::IsTrue() const
{
	size_t count = Size();
	for (size_t i = 0; i < count; i++) {
		if (!ElementAt(i).IsEmpty())
			return true;
	}

	return false;
}

bool
StringList::Contains(const String& string) const
{
	for (size_t i = 0; i < fSize; i++) {
		if (string == ElementAt(i))
			return true;
	}

	return false;
}

int
StringList::CompareWith(const StringList& other,
						bool ignoreTrailingEmptyStrings) const
{
	size_t commonSize = ignoreTrailingEmptyStrings
		? std::max(fSize, other.fSize)
		: std::min(fSize, other.fSize);
	for (size_t i = 0; i < commonSize; i++) {
		int compare = ElementAt(i).CompareWith(other.ElementAt(i));
		if (compare != 0)
			return compare;
	}

	return ignoreTrailingEmptyStrings ? 0 : (int)fSize - (int)other.fSize;
}

StringList&
StringList::Append(const String& string)
{
	_Detach(fSize + 1);
	fData->ConstructElement(fSize, string);
	fData->fSize = ++fSize;
	return *this;
}

StringList&
StringList::Append(const StringList& list)
{
	if (list.IsEmpty())
		return *this;

	if (IsEmpty()) {
		*this = list;
		return *this;
	}

	// Note: This also works for &list == this, since effectively the list
	// doesn't change until we change its size.
	size_t otherSize = list.Size();
	_Detach(fSize + otherSize);
	for (size_t i = 0; i < otherSize; i++)
		fData->ConstructElement(fSize + i, list.ElementAt(i));
	fSize += otherSize;
	fData->fSize = fSize;
	return *this;
}

String
StringList::Join() const
{
	size_t size = fSize;
	switch (size) {
		case 0:
			return String();
		case 1:
			return Head();
		default:
			break;
	}

	// compute result string length
	size_t resultLength = 0;
	for (size_t i = 0; i < size; i++)
		resultLength += ElementAt(i).Length();

	// allocate buffer and compute result
	String::Buffer* buffer = String::Buffer::Create(resultLength);
	char* destination = buffer->fString;
	for (size_t i = 0; i < size; i++) {
		String element = ElementAt(i);
		size_t length = element.Length();
		memcpy(destination, element.ToCString(), length);
		destination += length;
	}

	return String(buffer);
}

String
StringList::Join(const StringPart& separator) const
{
	if (separator.IsEmpty())
		return Join();

	size_t size = fSize;
	switch (size) {
		case 0:
			return String();
		case 1:
			return Head();
		default:
			break;
	}

	// compute result string length
	size_t resultLength = 0;
	for (size_t i = 0; i < size; i++)
		resultLength += ElementAt(i).Length();

	size_t separatorLength = separator.Length();
	resultLength += (size - 1) * separatorLength;

	// allocate buffer and compute result
	String::Buffer* buffer = String::Buffer::Create(resultLength);
	char* destination = buffer->fString;
	for (size_t i = 0; i < size; i++) {
		if (i > 0) {
			memcpy(destination, separator.Start(), separatorLength);
			destination += separatorLength;
		}

		String element = ElementAt(i);
		size_t length = element.Length();
		memcpy(destination, element.ToCString(), length);
		destination += length;
	}

	return String(buffer);
}

StringList
StringList::Multiply(const StringListList& listList)
{
	// Handle special cases quickly: empty list, single element.
	if (listList.empty())
		return StringList();

	size_t listCount = listList.size();
	if (listCount == 1)
		return listList.front();

	// Determine result size.
	size_t resultSize = 1;
	std::vector<const StringList*> lists(listCount);
	size_t listIndex = 0;
	for (StringListList::const_iterator it = listList.begin();
		 it != listList.end();
		 ++it) {
		resultSize *= it->Size();
		lists[listIndex++] = &*it;
	}
	if (resultSize == 0)
		return StringList();

	// Compute the result. We traverse the factor tree
	StringList resultList(resultSize);
	StringList factorList(listCount);
	std::vector<size_t> indexes(listCount, 0);
	listIndex = 0;
	size_t resultIndex = 0;
	for (;;) {
		// back-track, if through with the current list
		if (indexes[listIndex] == lists[listIndex]->Size()) {
			if (listIndex == 0)
				break;
			listIndex--;
			indexes[listIndex]++;
			continue;
		}

		factorList.SetElementAt(
			listIndex,
			lists[listIndex]->ElementAt(indexes[listIndex]));

		// descend
		if (listIndex < listCount - 1) {
			listIndex++;
			indexes[listIndex] = 0;
			continue;
		}

		// add element
		resultList.SetElementAt(resultIndex++, factorList.Join());

		indexes[listIndex]++;
	}

	return resultList;
}

bool
StringList::operator==(const StringList& other) const
{
	if (fSize != other.fSize)
		return false;
	for (size_t i = 0; i < fSize; i++) {
		if (ElementAt(i) != other.ElementAt(i))
			return false;
	}

	return true;
}

StringList&
StringList::operator=(const StringList& other)
{
	if (this != &other) {
		fData->Release();
		fData = other.fData;
		fData->Acquire();
		fOffset = other.fOffset;
		fSize = other.fSize;
	}

	return *this;
}

StringList::Data*
StringList::_CreateData(size_t size)
{
	if (size == 0) {
		Data::sEmptyData.Acquire();
		return &Data::sEmptyData;
	}

	return Data::Create(_CapacityForSize(size));
}

/**
 * Makes sure we are the only user of the data object (exception: 0 size), it is
 * large enough to contain \a newSize elements, and we're referencing offset 0.
 * If \a newSize is smaller than the current size, the excess elements are
 * destroyed and \c fSize will be adjusted accordingly. Otherwise the caller is
 * responsible for adjusting \c fSize and constructing the new elements.
 * @param newSize The new size of the list.
 */
void
StringList::_Detach(size_t newSize)
{
	// If we're the only user of the data object and the capacity and offset
	// won't change, just resize.
	if (fData->fReferenceCount == 1 && fOffset == 0
		&& _CapacityForSize(newSize) == fData->fCapacity) {
		if (newSize < fSize) {
			for (size_t i = newSize; i < fSize; i++)
				fData->DestroyElement(i);
			fData->fSize = newSize;
		}
		return;
	}

	Data* data = _CreateData(newSize);

	// Copying the elements cannot fail, so we don't need to worry about
	// exceptions.
	size_t toCopy = std::min(fSize, newSize);
	for (size_t i = 0; i < toCopy; i++)
		data->ConstructElement(i, ElementAt(i));
	data->fSize = toCopy;

	fData->Release();
	fData = data;
	fOffset = 0;
	fSize = toCopy;
}

/*static*/ inline size_t
StringList::_CapacityForSize(size_t size)
{
	size_t capacity = kMinCapacity;
	while (capacity < size)
		capacity <<= 1;
	return capacity;
}

} // namespace data
} // namespace ham
