/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_UTIL_SEQUENTIAL_SET_H
#define HAM_UTIL_SEQUENTIAL_SET_H


#include <set>
#include <vector>


namespace ham {
namespace util {


template<typename Element>
class SequentialSet {
public:
	typedef typename std::vector<Element>::const_iterator const_iterator;
	typedef const_iterator iterator;
	typedef typename std::vector<Element>::const_reference const_reference;

public:
	SequentialSet()
		:
		fSet(),
		fVector()
	{
	}

	bool insert(const Element& element)
	{
		return insert(end(), element);
	}

	bool insert(const iterator& position, const Element& element)
	{
		std::pair<typename std::set<Element>::iterator, bool> result
			= fSet.insert(element);
		if (!result.second)
			return false;

		try {
			size_t index = position - fVector.begin();
			fVector.insert(fVector.begin() + index, element);
			return true;
		} catch (...) {
			fSet.erase(result.first);
			throw;
		}
	}

	size_t size() const
	{
		return fVector.size();
	}

	bool empty() const
	{
		return fVector.empty();
	}

	const_reference at(size_t index) const
	{
		return fVector.at(index);
	}

	iterator begin() const
	{
		return fVector.begin();
	}

	iterator end() const
	{
		return fVector.end();
	}

private:
	std::set<Element>		fSet;
	std::vector<Element>	fVector;
};


}	// namespace util
}	// namespace ham


#endif	// HAM_UTIL_SEQUENTIAL_SET_H
