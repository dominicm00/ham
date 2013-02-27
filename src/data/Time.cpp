/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/Time.h"

#include <sys/time.h>
#include <time.h>


namespace ham {
namespace data {


/*static*/ Time
Time::Now()
{
	timeval time;
	gettimeofday(&time, NULL);
	return Time(time.tv_sec, time.tv_usec * 1000);

}


String
Time::ToString(bool includeNanoSeconds) const
{
// TODO: Support parameter
	char buffer[32];
	time_t time = Seconds();
	ctime_r(&time, buffer);

	size_t length = strlen(buffer);
	if (length > 0 && buffer[length - 1] == '\n')
		buffer[length - 1] = '\0';

	return String(buffer);
}


}	// namespace data
}	// namespace ham
