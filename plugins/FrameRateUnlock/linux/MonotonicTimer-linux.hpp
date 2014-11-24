#ifndef MONOTONICTIMER_HPP
#define MONOTONICTIMER_HPP

#include "../GameTimer.hpp"

/// <summary>
/// A frame rate timer that uses the CLOCK_MONOTONIC timer.
/// </summary>
class MonotonicTimer: public GameTimer
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="MonotonicTimer"/> class.
	/// </summary>
	MonotonicTimer();

	/// <summary>
	/// Gets the current value of the timer, in ticks.
	/// </summary>
	/// <returns>The current value of the timer, in ticks.</returns>
	uint64_t getTime();

	/// <summary>
	/// Gets the number of ticks in one second.
	/// </summary>
	/// <returns>The number of ticks in one second.</returns>
	uint64_t getTicksPerSecond() { return 1000000000; }

private:	
	/// <summary>
	/// Calculates the frequency of the timer.
	/// </summary>
	//void calculateFrequency();
};

#endif