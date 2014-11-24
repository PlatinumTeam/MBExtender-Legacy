#include "MachTimer-osx.hpp"
#include <mach/mach.h>

namespace
{
	uint64_t timespecToNsec(mach_timespec_t *ts);
}

MachTimer::MachTimer()
{
	host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &clockService);
}

MachTimer::~MachTimer()
{
	mach_port_deallocate(mach_task_self(), clockService);
}

uint64_t MachTimer::getTime()
{
	mach_timespec_t ts;
	clock_get_time(clockService, &ts);
	return timespecToNsec(&ts);
}

namespace
{
	uint64_t timespecToNsec(mach_timespec_t *ts)
	{
		return static_cast<uint64_t>(ts->tv_sec) * 1000000000U + ts->tv_nsec;
	}
}