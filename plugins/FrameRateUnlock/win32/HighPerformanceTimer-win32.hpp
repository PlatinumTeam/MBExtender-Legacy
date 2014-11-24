#ifndef HIGHPERFORMANCETIMER_HPP
#define HIGHPERFORMANCETIMER_HPP

#include "../GameTimer.hpp"

/// <summary>
/// A frame rate timer that uses the system's high performance counter.
/// </summary>
class HighPerformanceTimer: public GameTimer
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="HighPerformanceTimer"/> class.
	/// </summary>
	HighPerformanceTimer();

	/// <summary>
	/// Gets the current value of the timer, in ticks.
	/// </summary>
	/// <returns>The current value of the timer, in ticks.</returns>
	uint64_t getTime();

	/// <summary>
	/// Gets the number of ticks in one second.
	/// </summary>
	/// <returns>The number of ticks in one second.</returns>
	uint64_t getTicksPerSecond() { return frequency; }
	
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
};

#endif