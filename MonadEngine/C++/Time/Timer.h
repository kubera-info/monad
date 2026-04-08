// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>

namespace Monad::Time
{
	/// <summary>
	/// Performs timer operations
	/// Use g_timer to get the global instance
	/// </summary>
	struct Timer
	{
		Timer();
		/// <summary>
		/// resets the g_timer
		/// </summary>
		void Reset() noexcept;
		/// <summary>
		/// starts the g_timer
		/// </summary>
		void Start() noexcept;
		/// <summary>
		/// stop (or pause) the g_timer
		/// </summary>
		void Stop() noexcept;
		/// <summary>
		/// Current time in seconds, when the timer is paused, the value is not changing
		/// </summary>
		/// <param name="time">
		/// Returns elapsed time in seconds since last Reset().
		/// If the timer is stopped, the value does not change.
		/// </param>
		void GetTimeValue(
			double& time
		) noexcept; // get the time value at once

		// Limit the current thread to one processor (the current one). This ensures that timing code runs
		// on only one processor, and will not suffer any ill effects from power management.

	private:
		LARGE_INTEGER GetAdjustedCurrentTime() const;
		void QueryPerformanceCounter(
			LARGE_INTEGER& lpPerformanceCount
		) const;

		bool m_timerStopped;
		long long m_llQPFTicksPerSec, m_llStopTime, m_llBaseTime;
	};

	extern Timer g_timer;
}