/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_DATA_TIME_HPP
#define HAM_DATA_TIME_HPP

#include <stddef.h>
#include <stdint.h>

#include "String.hpp"

namespace ham::data
{

class Time
{
  public:
	static const uint32_t kNanoFactor = 1000000000;

  public:
	inline Time();
	inline Time(int64_t nanoSeconds);
	inline Time(uint32_t seconds, uint32_t nanoSeconds);

	bool IsValid() const { return fNanoSeconds >= 0; }

	uint32_t Seconds() const { return fNanoSeconds / kNanoFactor; }
	uint32_t NanoSeconds() const { return fNanoSeconds % kNanoFactor; }

	String ToString(bool includeNanoSeconds = true) const;

	static Time Now();

	inline bool operator==(const Time& other) const;
	bool operator!=(const Time& other) const { return !(*this == other); }

	inline bool operator<(const Time& other) const;
	bool operator>(const Time& other) const { return other < *this; }
	bool operator<=(const Time& other) const { return !(*this > other); }
	bool operator>=(const Time& other) const { return other <= *this; }

  private:
	int64_t fNanoSeconds;
	// nanoseconds since the Epoch
};

Time::Time()
	: fNanoSeconds(-1)
{
}

Time::Time(int64_t nanoSeconds)
	: fNanoSeconds(nanoSeconds)
{
}

Time::Time(uint32_t seconds, uint32_t nanoSeconds)
	: fNanoSeconds((int64_t)seconds * 1000000000 + nanoSeconds)
{
}

bool
Time::operator==(const Time& other) const
{
	return fNanoSeconds == other.fNanoSeconds;
}

bool
Time::operator<(const Time& other) const
{
	return fNanoSeconds < other.fNanoSeconds;
}

} // namespace ham::data

#endif // HAM_DATA_TIME_HPP
