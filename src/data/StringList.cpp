/*
 * Copyright 2012, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "StringList.h"

#include <algorithm>


namespace ham {
namespace data {


StringList::Data StringList::sEmptyData(0);
const StringList StringList::kTrue("1");
const StringList StringList::kFalse;

static const size_t kMinCapacity = 8;


StringList::StringList()
	:
	fData(&sEmptyData),
	fOffset(0),
	fSize(0)
{
	fData->Acquire();
}


StringList::StringList(const String& string)
	:
	fData(_CreateData(1)),
	fOffset(0),
	fSize(1)
{
	fData->ConstructElement(0, string);
	fData->fSize = 1;
}


StringList::StringList(const StringList& other)
	:
	fData(other.fData),
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
StringList::Contains(const String& string) const
{
	for (size_t i = 0; i < fSize; i++) {
		if (string == ElementAt(i))
			return true;
	}

	return false;
}


int
StringList::CompareWith(const StringList& other) const
{
	size_t commonSize = std::min(fSize, other.fSize);
	for (size_t i = 0; i < commonSize; i++) {
		int compare = ElementAt(i).CompareWith(other.ElementAt(i));
		if (compare != 0)
			return compare;
	}

	return (int)fSize - (int)other.fSize;
}


void
StringList::Append(const String& string)
{
	_Detach(fSize + 1);
	fData->ConstructElement(fSize, string);
	fData->fSize = ++fSize;
}


void
StringList::Append(const StringList& list)
{
	if (list.IsEmpty())
		return;

	if (IsEmpty()) {
		*this = list;
		return;
	}

	// Note: This also works for &list == this, since effectively the list
	// doesn't change until we change its size.
	size_t otherSize = list.Size();
	_Detach(fSize + otherSize);
	for (size_t i = 0; i < otherSize; i++)
		fData->ConstructElement(fSize + i, list.ElementAt(i));
	fSize += otherSize;
	fData->fSize = fSize;
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
		sEmptyData.Acquire();
		return &sEmptyData;
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
	if (fData->fReferenceCount > 1 && fOffset == 0
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


}	// namespace data
}	// namespace ham
