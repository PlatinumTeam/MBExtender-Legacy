/* Unlocks the game's frame rate.
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
 * There are also issues with frame rate on the Mac version of the game.
 *
 * This plugin fixes the issue by overriding the game's timestep code so that it
 * uses the highest-resolution timer available on the user's system.
*/

#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>
#include <PluginLoader/PluginInterface.h>
#include <memory>
#include "GameTimer.hpp"

#if defined(_WIN32)
 #include "win32/HighPerformanceTimer-win32.hpp"
 #include "win32/MultimediaTimer-win32.hpp"
#elif defined(__MACH__)
 #include "osx/MachTimer-osx.hpp"
#elif defined(__linux)
 #include "linux/MonotonicTimer-linux.hpp"
#endif

namespace
{
	std::unique_ptr<GameTimer> timer;            // Active frame rate timer
	const uint32_t UpdateInterval = 1;           // Minimum amount of milliseconds between updates
	uint64_t lastTime;                           // Last frame time
	uint64_t accumulator = 0;                    // Time accumulator
	bool enabled = true;                         // If set to false, fall back to old timing system
		
	/// <summary>
	/// Detects the best timer to use for measuring frame time and stores the resulting timer object.
	/// </summary>
	void detectTimer()
	{
#if defined(_WIN32)
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
#elif defined(__MACH__)
		MachTimer *mt = new MachTimer();
		timer = std::unique_ptr<GameTimer>(mt);
		TGE::Con::printf("FrameRateUnlock: Using Mach SYSTEM_CLOCK");
#elif defined(__linux)
		MonotonicTimer *mt = new MonotonicTimer();
		timer = std::unique_ptr<GameTimer>(mt);
		TGE::Con::printf("FrameRateUnlock: Using CLOCK_MONOTONIC");
#endif
		TGE::Con::printf("FrameRateUnlock: Timer frequency = %d", static_cast<int>(timer->getTicksPerSecond()));
		lastTime = timer->getTime();
	}
}

// TimeManager::process() override for using a higher-resolution timer
TorqueOverride(void, TimeManager::process, (), originalProcess)
{
	if (!enabled || !timer)
	{
		originalProcess();
		return;
	}

	// Measure the amount of time since the last cycle and add it to the accumulator
	uint64_t newTime = timer->getTime();
	accumulator += newTime - lastTime;
	lastTime = newTime;

	// Check if enough ms have accumulated
	U32 accumulatorMs = static_cast<U32>(accumulator * 1000 / timer->getTicksPerSecond());
	if (accumulatorMs >= UpdateInterval)
	{
		// Post a time update event
		TGE::TimeEvent ev;
		ev.deltaTime = accumulatorMs;
		TGE::Game->postEvent(ev);
		accumulator -= accumulatorMs * timer->getTicksPerSecond() / 1000;
	}
}

// Console function to enable/disable the plugin
ConsoleFunction(enableFrameRateUnlock, void, 2, 2, "enableFrameRateUnlock(enabled)")
{
	int newEnabled = atoi(argv[1]);
	enabled = (newEnabled != 0);
	if (enabled)
	{
		lastTime = timer->getTime();
		TGE::Con::printf("Frame rate unlock enabled");
	}
	else
	{
		TGE::Con::printf("Frame rate unlock disabled");
	}
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
	detectTimer();
}

PLUGINCALLBACK void engineShutdown(PluginInterface *plugin)
{
}