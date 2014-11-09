#ifndef HIGHPERFORMANCETIMER_HPP
#define HIGHPERFORMANCETIMER_HPP

#include "FrameRateTimer.hpp"

/// <summary>
/// A frame rate timer that uses the system's high performance counter.
/// </summary>
class HighPerformanceTimer: public FrameRateTimer
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="HighPerformanceTimer"/> class.
	/// </summary>
	HighPerformanceTimer();
	
	/// <summary>
	/// Gets the frequency of the timer in ticks per second.
	/// </summary>
	/// <returns>The frequency of the timer in ticks per second, or 0 if the timer is not supported by the user's system.</returns>
	uint64_t getFrequency() const { return frequency; }

	/// <summary>
	/// Gets the amount of time elapsed since the last frame, in milliseconds.
	/// </summary>
	/// <returns>The amount of time elapsed since the last frame, in milliseconds.</returns>
	uint32_t getElapsedTimeMs();

	/// <summary>
	/// Updates the timer after a frame update.
	/// </summary>
	/// <param name="elapsed">The amount of time elapsed since the last frame, in milliseconds.</param>
	void update(uint32_t elapsed);
	
	/// <summary>
	/// Determines whether the high performance timer is supported by the user's system.
	/// </summary>
	/// <returns><c>true</c> if the high performance timer is supported.</returns>
	static bool isSupported();

private:	
	/// <summary>
	/// Calculates the frequency of the system's high performance timer.
	/// </summary>
	void calculateFrequency();

	uint64_t frequency; // Frequency in ticks per second
	uint64_t lastTime;  // Value of timer at last call to getElapsedTimeMs in ticks
};

#endif