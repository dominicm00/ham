/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "tests/TimeTest.h"

#include <sys/time.h>

#include "data/Time.h"


namespace ham {
namespace tests {


using data::Time;


#define TIME_COMPARE_WORK(time1, time2, expected)				\
	HAM_TEST_EQUAL(time1 == time2, expected == 0)				\
	HAM_TEST_EQUAL(time1 != time2, expected != 0)				\
	HAM_TEST_EQUAL(time1 < time2, expected < 0)					\
	HAM_TEST_EQUAL(time1 > time2, expected > 0)					\
	HAM_TEST_EQUAL(time1 <= time2, expected <= 0)				\
	HAM_TEST_EQUAL(time1 >= time2, expected >= 0)

#define TIME_COMPARE(time1, time2, expected)				\
	TIME_COMPARE_WORK(time1, time2, expected)				\
	TIME_COMPARE_WORK(time2, time1, -expected)


void
TimeTest::Constructor()
{
	// default constructor
	{
		Time time;
		HAM_TEST_VERIFY(!time.IsValid())
	}

	// (int64_t) constructor
	{
		Time time((int64_t)-1);
		HAM_TEST_VERIFY(!time.IsValid())
	}

	{
		struct TestData {
			int64_t		nanoSecondsSinceEpoch;
			uint32_t	seconds;
			uint32_t	nanoSeconds;
		};

		const TestData testData[] = {
			{ (int64_t)0,			0,				0 },
			{ (int64_t)7,			0,				7 },
			{ (int64_t)3351345,		0,				3351345 },
			{ (int64_t)999999999,	0,				999999999 },
			{ (int64_t)1000000000,	1,				0 },
			{ (int64_t)13999999999,	13,				999999999 },
			{ (int64_t)~(uint32_t)0 * 1000000000 + 452345235, ~(uint32_t)0,
				(uint32_t)452345235 },
		};

		for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
			Time time(testData[i].seconds, testData[i].nanoSeconds);
			HAM_TEST_VERIFY(time.IsValid())
			HAM_TEST_EQUAL(time.Seconds(), testData[i].seconds)
			HAM_TEST_EQUAL(time.NanoSeconds(), testData[i].nanoSeconds)
		}
	}

	// (uint32_t, uint32_t) constructor
	{
		struct TestData {
			uint32_t	seconds;
			uint32_t	nanoSeconds;
		};

		const TestData testData[] = {
			{ 0,			0 },
			{ 0,			264597409 },
			{ 0,			999999999 },
			{ 1,			0 },
			{ 1,			999999999 },
			{ 42,			435264264 },
			{ ~(uint32_t)0,	435264264 },
			{ ~(uint32_t)0,	999999999 },
		};

		for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
			Time time(testData[i].seconds, testData[i].nanoSeconds);
			HAM_TEST_VERIFY(time.IsValid())
			HAM_TEST_EQUAL(time.Seconds(), testData[i].seconds)
			HAM_TEST_EQUAL(time.NanoSeconds(), testData[i].nanoSeconds)
		}
	}
}


void
TimeTest::Comparison()
{
	struct TestData {
		int64_t	time1;
		int64_t	time2;
		int		compare;
	};

	const TestData testData[] = {
		{ 0,			0,				0 },
		{ 9,			0,				1 },
		{ 9,			7,				1 },
		{ 9,			9,				0 },
		{ 999999999,	0,				1 },
		{ 999999999,	7,				1 },
		{ 999999999,	999999999,		0 },
		{ 1000000000,	0,				1 },
		{ 1000000000,	999999999,		1 },
		{ 1000000000,	1000000000,		0 },
		{ 1000000001,	1000000000,		1 },
		{ 1999999999,	1000000000,		1 },
		{ 1999999999,	1999999999,		0 },
		{ (int64_t)~(uint32_t)0 * 1000000000 + 999999999,	0,		1 },
		{ (int64_t)~(uint32_t)0 * 1000000000 + 999999999,	999999999999,	1 },
		{ (int64_t)~(uint32_t)0 * 1000000000 + 999999999,
			(int64_t)~(uint32_t)0 * 1000000000 + 999999998,	1 },
		{ (int64_t)~(uint32_t)0 * 1000000000 + 999999999,
			(int64_t)~(uint32_t)0 * 1000000000 + 999999999,	0 },
	};

	for (size_t i = 0; i < sizeof(testData) / sizeof(testData[0]); i++) {
		Time time1(testData[i].time1);
		Time time2(testData[i].time2);
		HAM_TEST_ADD_INFO(
			TIME_COMPARE(time1, time2, testData[i].compare),
			"time1: %lld, time2: %lld", (long long)testData[i].time1,
			(long long)testData[i].time2)
	}
}


void
TimeTest::Now()
{
// TODO: Platform specific!
	// Difficult to test. We fetch the time before and after and see, if the
	// Time::Now() lies in between.
	timeval startTime;
	gettimeofday(&startTime, NULL);

	Time time = Time::Now();

	timeval endTime;
	gettimeofday(&endTime, NULL);

	HAM_TEST_VERIFY(startTime.tv_sec <= time.Seconds())
	HAM_TEST_VERIFY(endTime.tv_sec >= time.Seconds())

	if (startTime.tv_sec == time.Seconds())
		HAM_TEST_VERIFY(startTime.tv_usec * 1000 <= time.NanoSeconds())

	if (endTime.tv_sec == time.Seconds())
		HAM_TEST_VERIFY(endTime.tv_usec * 1000 >= time.NanoSeconds())
}


} // namespace tests
} // namespace ham
