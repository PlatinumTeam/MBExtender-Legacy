#include <Windows.h>
#include "HighPerformanceTimer-win32.hpp"

HighPerformanceTimer::HighPerformanceTimer()
	: frequency(0)
{
	calculateFrequency();
}

uint64_t HighPerformanceTimer::getTime()
{
	LARGE_INTEGER currentTime;
	if (frequency == 0 || !QueryPerformanceCounter(&currentTime))
		return 0;
	return currentTime.QuadPart;
}

bool HighPerformanceTimer::isSupported()
{
	LARGE_INTEGER frequency;
	return (QueryPerformanceFrequency(&frequency) != 0);
}

void HighPerformanceTimer::calculateFrequency()
{
	LARGE_INTEGER result;
	if (QueryPerformanceFrequency(&result))
		frequency = result.QuadPart;
}