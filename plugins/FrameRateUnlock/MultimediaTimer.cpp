#include <Windows.h>
#include "MultimediaTimer.hpp"

MultimediaTimer::MultimediaTimer()
	: lastTime(0), resolution(1)
{
	applyFinestResolution();
	lastTime = timeGetTime();
}

MultimediaTimer::~MultimediaTimer()
{
	restoreResolution();
}

uint32_t MultimediaTimer::getElapsedTimeMs()
{
	uint32_t currentTime = timeGetTime();
	return currentTime - lastTime;
}

void MultimediaTimer::update(uint32_t elapsed)
{
	lastTime += elapsed;
}

void MultimediaTimer::applyFinestResolution()
{
	// Start at 1 and keep increasing until Windows gives an OK
	resolution = 1;
	while (timeBeginPeriod(resolution) != TIMERR_NOERROR)
		resolution++;
}

void MultimediaTimer::restoreResolution()
{
	timeEndPeriod(resolution);
}