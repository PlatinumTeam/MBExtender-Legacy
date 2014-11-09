#include <Windows.h>
#include "HighPerformanceTimer.hpp"

HighPerformanceTimer::HighPerformanceTimer()
	: frequency(0), lastTime(0)
{
	calculateFrequency();
	
	LARGE_INTEGER currentTime;
	if (frequency != 0 && QueryPerformanceCounter(&currentTime))
		lastTime = currentTime.QuadPart;
}

uint32_t HighPerformanceTimer::getElapsedTimeMs()
{
	LARGE_INTEGER currentTime;
	if (frequency == 0 || !QueryPerformanceCounter(&currentTime))
		return 0;
	uint64_t diff = currentTime.QuadPart - lastTime;
	uint64_t msec = diff * 1000 / frequency; // Convert to milliseconds before dividing by frequency to maximize precision
	return static_cast<uint32_t>(msec);
}

void HighPerformanceTimer::update(uint32_t elapsed)
{
	if (frequency != 0)
		lastTime += elapsed * frequency / 1000;
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
	else
		frequency = 0;
}