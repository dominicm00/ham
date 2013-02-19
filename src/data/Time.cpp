/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "data/Time.h"

#include <sys/time.h>


namespace ham {
namespace data {


/*static*/ Time
Time::Now()
{
	timeval time;
	gettimeofday(&time, NULL);
	return Time(time.tv_sec, time.tv_usec * 1000);

}


}	// namespace data
}	// namespace ham
