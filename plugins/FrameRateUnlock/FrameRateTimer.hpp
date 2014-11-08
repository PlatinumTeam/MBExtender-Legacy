#ifndef FRAMERATETIMER_HPP
#define FRAMERATETIMER_HPP

#include <cstdint>

/// <summary>
/// Interface for a class which represents a timer accurate enough to be used as a frame rate throttle.
/// </summary>
class FrameRateTimer
{
public:
	/// <summary>
	/// Finalizes an instance of the <see cref="FrameRateTimer"/> class.
	/// </summary>
	virtual ~FrameRateTimer() { }
	
	/// <summary>
	/// Gets the amount of time elapsed since the last time <see cref="update"/> was called, in milliseconds.
	/// </summary>
	/// <returns>The amount of time elapsed since the last time <see cref="update"/> was called, in milliseconds.</returns>
	virtual uint32_t getElapsedTimeMs() = 0;
	
	/// <summary>
	/// Updates the timer such that subsequent calls to <see cref="getElapsedTimeMs"/> will return the amount of time elapsed since the most recent update.
	/// </summary>
	virtual void update() = 0;
};

#endif