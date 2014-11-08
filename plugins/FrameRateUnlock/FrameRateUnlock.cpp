// Unlocks the game's framerate in Windows.

#ifndef WIN32
#error FrameRateUnlock is specific to the Windows version of the game.
#endif

#include <TorqueLib/TGE.h>
#include <TorqueLib/QuickOverride.h>
#include <PluginLoader/PluginInterface.h>
#include <Windows.h>
#include <memory>
#include "FrameRateTimer.hpp"
#include "HighPerformanceTimer.hpp"
#include "MultimediaTimer.hpp"

namespace
{
	std::unique_ptr<FrameRateTimer> timer;       // Active frame rate timer
	const uint32_t MinUpdateInterval = 3;        // Minimum value for updateInterval. GG used 3 in the game code
	uint32_t updateInterval = MinUpdateInterval; // Update interval in milliseconds
	                                             //     TODO: For some reason, setting this to 1 slows down physics if the HPT is active. Figure out why.
	bool enabled = true;                         // If set to false, fall back to old timing system
		
	/// <summary>
	/// Detects the best timer to use for measuring frame time and stores the resulting timer object.
	/// </summary>
	void detectTimer()
	{
		// High-performance timer takes precedence over multimedia timer
		if (HighPerformanceTimer::isSupported())
		{
			HighPerformanceTimer *hpt = new HighPerformanceTimer();
			timer = std::unique_ptr<FrameRateTimer>(hpt);
			TGE::Con::printf("FrameRateUnlock: Detected high-performance timer with frequency = %d", static_cast<int>(hpt->getFrequency()));
		}
		else
		{
			// Fall back to multimedia timer
			MultimediaTimer *mm = new MultimediaTimer();
			timer = std::unique_ptr<FrameRateTimer>(mm);
			TGE::Con::printf("FrameRateUnlock: Falling back to multimedia timer with resolution = %d", mm->getResolution());
		}
	}
}

TorqueOverride(void, TimeManager::process, (), originalProcess)
{
	if (!enabled)
	{
		originalProcess();
		return;
	}

	// Only update if at least updateInterval milliseconds have passed
	uint32_t elapsed = timer->getElapsedTimeMs();
	if (elapsed >= updateInterval)
	{
		timer->update();

		// Post a time update event
		TGE::TimeEvent ev;
		ev.deltaTime = elapsed;
		TGE::Game->postEvent(ev);
	}
}

// Console function to enable/disable the plugin
ConsoleFunction(enableFrameRateUnlock, void, 2, 2, "enableFrameRateUnlock(enabled)")
{
	int newEnabled = 0;
	if (sscanf_s(argv[1], "%d", &newEnabled) != 1)
		return;
	enabled = (newEnabled != 0);
	if (enabled)
		TGE::Con::printf("Frame rate unlock enabled");
	else
		TGE::Con::printf("Frame rate unlock disabled");
}

// Console function to set the update interval
ConsoleFunction(setTickInterval, void, 2, 2, "setTickInterval(ms)")
{
	int newInterval = 0;
	if (sscanf_s(argv[1], "%d", &newInterval) != 1)
	{
		TGE::Con::errorf("Invalid tick interval value");
		return;
	}
	updateInterval = max(MinUpdateInterval, newInterval);
}

PLUGINCALLBACK void preEngineInit(PluginInterface *plugin)
{
}

PLUGINCALLBACK void postEngineInit(PluginInterface *plugin)
{
	detectTimer();
}