#include <Windows.h>
#include "MultimediaTimer-win32.hpp"

MultimediaTimer::MultimediaTimer()
	: resolution(1)
{
	applyFinestResolution();
}

MultimediaTimer::~MultimediaTimer()
{
	restoreResolution();
}

uint64_t MultimediaTimer::getTime()
{
	return timeGetTime();
}

void MultimediaTimer::applyFinestResolution()
{
	// Start at 1 and keep increasing until Windows gives an OK
	// TODO: Call timeGetDevCaps()
	resolution = 1;
	while (timeBeginPeriod(resolution) != TIMERR_NOERROR)
		resolution++;
}

void MultimediaTimer::restoreResolution()
{
	timeEndPeriod(resolution);
}