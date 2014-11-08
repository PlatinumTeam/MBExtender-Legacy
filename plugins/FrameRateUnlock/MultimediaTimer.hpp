#ifndef MULTIMEDIATIMER_HPP
#define MULTIMEDIATIMER_HPP

#include "FrameRateTimer.hpp"

/// <summary>
/// A frame rate timer that uses the system's multimedia timer.
/// </summary>
class MultimediaTimer: public FrameRateTimer
{
public:	
	/// <summary>
	/// Initializes a new instance of the <see cref="MultimediaTimer"/> class.
	/// The system's multimedia timer will be adjusted to the finest resolution possible.
	/// </summary>
	MultimediaTimer();
	
	/// <summary>
	/// Finalizes an instance of the <see cref="MultimediaTimer"/> class.
	/// The system's multimedia timer resolution will be reset.
	/// </summary>
	~MultimediaTimer();
	
	/// <summary>
	/// Gets the resolution of the timer, in milliseconds.
	/// </summary>
	/// <returns>The resolution of the timer, in milliseconds.</returns>
	int getResolution() const { return resolution; }
		
	/// <summary>
	/// Gets the amount of time elapsed since the last time <see cref="update"/> was called, in milliseconds.
	/// </summary>
	/// <returns>The amount of time elapsed since the last time <see cref="update"/> was called, in milliseconds.</returns>
	uint32_t getElapsedTimeMs();

	/// <summary>
	/// Updates the timer such that subsequent calls to <see cref="getElapsedTimeMs"/> will return the amount of time elapsed since the most recent update.
	/// </summary>
	void update();

private:	
	/// <summary>
	/// Adjusts the resolution of the system multimedia timer to be as fine as possible.
	/// </summary>
	void applyFinestResolution();
	
	/// <summary>
	/// Restores the resolution of the system multimedia timer.
	/// </summary>
	void restoreResolution();

	int resolution;    // Resolution in milliseconds
	uint32_t lastTime; // Time of last call to getElapsedTimeMs
};

#endif