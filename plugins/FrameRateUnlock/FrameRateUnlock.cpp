/* Windows only: unlocks the game's frame rate.
 *
 * By default, the Windows build of the game uses GetTickCount() in the timing
 * loop to determine when to send out a time event. The problem with this is
 * that GetTickCount() only has a resolution of 15.625ms on most systems,
 * resulting in an unintentional frame rate cap of 64fps on Windows.
 *
 * (Interestingly, this problem disappears if the Windows version of the game
 * is run under WINE. This is likely because WINE's GetTickCount()
 * implementation is more precise.)
 *
 * This plugin fixes the issue by overriding the game's timestep code so that it
 * uses the highest-resolution timer available on the user's system. If the
 * system has a high-performance counter (QueryPerformanceCounter, etc.)
 * available, then that will be used, otherwise the system multimedia timer will
 * be used with as high of a resolution as possible.
*/

#ifndef WIN32
#error FrameRateUnlock is specific to the Windows version of the game.
#endif

#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>
#include <PluginLoader/PluginInterface.h>
#include <Windows.h>
#include <memory>
#include "GameTimer.hpp"
#include "HighPerformanceTimer.hpp"
#include "MultimediaTimer.hpp"

namespace
{
	std::unique_ptr<GameTimer> timer;       // Active frame rate timer
	const uint32_t MinUpdateInterval = 1;        // Minimum value that updateInterval can have.
	uint32_t updateInterval = MinUpdateInterval; // Update interval in milliseconds
	uint64_t lastTime;                           // Last frame time
	double timeScale = 1.0;                      // Time multiplier
	double accumulator = 0;                      // Time accumulator (used for time scaling - if this becomes >= 1, then an update can happen)
	bool enabled = true;                         // If set to false, fall back to old timing system
		
	/// <summary>
	/// Detects the best timer to use for measuring frame time and stores the resulting timer object.
	/// </summary>
	void detectTimer()
	{
#ifndef FORCE_MMTIMER
		// High-performance timer takes precedence over multimedia timer
		if (HighPerformanceTimer::isSupported())
		{
			HighPerformanceTimer *hpt = new HighPerformanceTimer();
			timer = std::unique_ptr<GameTimer>(hpt);
			TGE::Con::printf("FrameRateUnlock: Using high-performance timer");
		}
		else
#endif
		{
			// Fall back to multimedia timer
			MultimediaTimer *mm = new MultimediaTimer();
			timer = std::unique_ptr<GameTimer>(mm);
			TGE::Con::printf("FrameRateUnlock: Using multimedia timer");
		}
		TGE::Con::printf("FrameRateUnlock: Timer resolution = %d", static_cast<int>(timer->getTicksPerSecond()));
		lastTime = timer->getTime();
	}
}

// TimeManager::process() override for using a higher-resolution timer
TorqueOverride(void, TimeManager::process, (), originalProcess)
{
	if (!enabled)
	{
		originalProcess();
		return;
	}

	// Only update if at least updateInterval milliseconds have passed
	uint64_t elapsedMs = (timer->getTime() - lastTime) * 1000 / timer->getTicksPerSecond();
	if (elapsedMs >= updateInterval)
	{
		// Add a whole number of milliseconds onto the reference time
		lastTime += elapsedMs * timer->getTicksPerSecond() / 1000;

		// Add time to the accumulator
		accumulator += timeScale * elapsedMs;
		if (accumulator >= 1.0)
		{
			// At least 1ms accumulated - post a time update event
			TGE::TimeEvent ev;
			ev.deltaTime = static_cast<U32>(accumulator);
			TGE::Game->postEvent(ev);
			accumulator -= ev.deltaTime;
		}
	}
}

// Console function to enable/disable the plugin
ConsoleFunction(enableFrameRateUnlock, void, 2, 2, "enableFrameRateUnlock(enabled)")
{
	int newEnabled = atoi(argv[1]);
	enabled = (newEnabled != 0);
	if (enabled)
		TGE::Con::printf("Frame rate unlock enabled");
	else
		TGE::Con::printf("Frame rate unlock disabled");
}

// Console function to set the update interval
ConsoleFunction(setTickInterval, void, 2, 2, "setTickInterval(msec)")
{
	int newInterval = atoi(argv[1]);
	updateInterval = max(MinUpdateInterval, newInterval);
}

// Console function to set the time scale
ConsoleFunction(setTimeScale, void, 2, 2, "setTimeScale(scale)")
{
	double newScale = atof(argv[1]);
	if (newScale > 0)
	{
		timeScale = newScale;
		TGE::Con::printf("Set time scale to %f", timeScale);
	}
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
	detectTimer();
}