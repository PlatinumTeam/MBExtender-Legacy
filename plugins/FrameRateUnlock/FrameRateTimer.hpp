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
	/// Gets the amount of time elapsed since the last frame, in milliseconds.
	/// </summary>
	/// <returns>The amount of time elapsed since the last frame, in milliseconds.</returns>
	virtual uint32_t getElapsedTimeMs() = 0;
	
	/// <summary>
	/// Updates the timer after a frame update.
	/// </summary>
	/// <param name="elapsed">The amount of time elapsed since the last frame, in milliseconds.</param>
	virtual void update(uint32_t elapsed) = 0;
};

#endif