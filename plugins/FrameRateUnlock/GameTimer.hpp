#ifndef GAMETIMER_HPP
#define GAMETIMER_HPP

#include <cstdint>

/// <summary>
/// Interface for a class which represents a high-resolution timer.
/// </summary>
class GameTimer
{
public:
	/// <summary>
	/// Finalizes an instance of the <see cref="GameTimer"/> class.
	/// </summary>
	virtual ~GameTimer() { }
	
	/// <summary>
	/// Gets the current value of the timer, in ticks.
	/// </summary>
	/// <returns>The current value of the timer, in ticks.</returns>
	virtual uint64_t getTime() = 0;
	
	/// <summary>
	/// Gets the number of ticks in one second.
	/// </summary>
	/// <returns>The number of ticks in one second.</returns>
	virtual uint64_t getTicksPerSecond() = 0;
};

#endif