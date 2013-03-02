/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_SEQUENTIAL_SET_H
#define HAM_UTIL_SEQUENTIAL_SET_H


#include <set>
#include <vector>

#include "util/Exception.h"


namespace ham {
namespace util {


template<typename Element>
class SequentialSet {
public:
	class Iterator;

public:
	SequentialSet()
		:
		fSet(),
		fVector()
	{
	}

	bool Append(const Element& element)
	{
		return Insert(element, Size());
	}

	bool Insert(const Element& element, size_t index)
	{
		if (index > Size())
			throw Exception("SequentialSet::Insert(): index out of bounds");

		std::pair<typename std::set<Element>::iterator, bool> result
			= fSet.insert(element);
		if (!result.second)
			return false;

		try {
			fVector.insert(fVector.begin() + index, element);
			return true;
		} catch (...) {
			fSet.erase(result.first);
			throw;
		}
	}

	size_t Size() const
	{
		return fVector.size();
	}

	bool IsEmpty() const
	{
		return fVector.empty();
	}

	const Element& ElementAt(size_t index) const
	{
		return fVector.at(index);
	}

	const Element& Head() const
	{
		return ElementAt(0);
	}

	Iterator GetIterator() const
	{
		return Iterator(*this);
	}

private:
	std::set<Element>		fSet;
	std::vector<Element>	fVector;
};


template<typename Element>
class SequentialSet<Element>::Iterator {
public:
	Iterator()
		:
		fSet(NULL),
		fNextIndex(0)
	{
	}

	Iterator(const SequentialSet& set)
		:
		fSet(&set),
		fNextIndex(0)
	{
	}

	bool HasNext() const
	{
		return fSet != NULL && fNextIndex < fSet->Size();
	}

	const Element& Next()
	{
		return fSet->ElementAt(fNextIndex++);
	}

	bool operator==(const Iterator& other) const
	{
		return fSet == other.fSet && fNextIndex == other.fNextIndex;
	}

	bool operator!=(const Iterator& other) const
	{
		return !(*this == other);
	}

private:
	const SequentialSet*	fSet;
	size_t					fNextIndex;
};


}	// namespace util
}	// namespace ham


#endif	// HAM_UTIL_SEQUENTIAL_SET_H
