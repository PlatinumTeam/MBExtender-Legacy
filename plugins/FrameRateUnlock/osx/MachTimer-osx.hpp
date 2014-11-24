#ifndef MACHTIMER_OSX_HPP
#define MACHTIMER_OSX_HPP

#include "../GameTimer.hpp"
#include <mach/clock.h>

/// <summary>
/// A frame rate timer that uses the Mach high-resolution timer.
/// </summary>
class MachTimer: public GameTimer
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="MachTimer"/> class.
	/// </summary>
	MachTimer();

	~MachTimer();

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
	clock_serv_t clockService;
};

#endif