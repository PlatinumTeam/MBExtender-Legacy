#include "MonotonicTimer-linux.hpp"
#include <time.h>

namespace
{
	uint64_t timespecToNsec(struct timespec *ts);
}

MonotonicTimer::MonotonicTimer()
{
	//calculateFrequency();
}

uint64_t MonotonicTimer::getTime()
{
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts))
		return 0;
	return timespecToNsec(&ts);
}

namespace
{
	uint64_t timespecToNsec(struct timespec *ts)
	{
		return static_cast<uint64_t>(ts->tv_sec) * 1000000000U + ts->tv_nsec;
	}
}