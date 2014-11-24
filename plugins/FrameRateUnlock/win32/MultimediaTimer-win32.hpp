#ifndef MULTIMEDIATIMER_HPP
#define MULTIMEDIATIMER_HPP

#include "../GameTimer.hpp"

/// <summary>
/// A frame rate timer that uses the system's multimedia timer.
/// </summary>
class MultimediaTimer: public GameTimer
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
	/// Gets the current value of the timer, in ticks.
	/// </summary>
	/// <returns>The current value of the timer, in ticks.</returns>
	virtual uint64_t getTime();

	/// <summary>
	/// Gets the number of ticks in one second.
	/// </summary>
	/// <returns>The number of ticks in one second.</returns>
	virtual uint64_t getTicksPerSecond() { return 1000 / resolution; }

private:	
	/// <summary>
	/// Adjusts the resolution of the system multimedia timer to be as fine as possible.
	/// </summary>
	void applyFinestResolution();
	
	/// <summary>
	/// Restores the resolution of the system multimedia timer.
	/// </summary>
	void restoreResolution();

	// Resolution in milliseconds
	int resolution;
};

#endif