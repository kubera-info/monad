// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Timer.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Time
{
	/// <summary>
	/// High–precision timer based on QueryPerformanceCounter.
	/// Used as a global time source for rendering and animation systems.
	/// </summary>
	Timer::Timer() :
		m_timerStopped{ true },
		m_llQPFTicksPerSec{},
		m_llStopTime{},
		m_llBaseTime{}
	{
		// Query system performance frequency (ticks per second)
		LARGE_INTEGER qwTicksPerSec{};
		if (QueryPerformanceFrequency(&qwTicksPerSec) == FALSE)
			THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"Sys Timer Ctor");

		m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
	}

	/// <summary>
	/// Resets the timer and starts measuring time from zero.
	/// </summary>
	void Timer::Reset() noexcept
	{
		LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

		m_llBaseTime = qwTime.QuadPart;
		m_llStopTime = 0;
		m_timerStopped = false;
	}

	/// <summary>
	/// Resumes timer if it was previously stopped.
	/// </summary>
	void Timer::Start() noexcept
	{
		LARGE_INTEGER qwTime{};
		QueryPerformanceCounter(qwTime);

		if (m_timerStopped)
			m_llBaseTime += qwTime.QuadPart - m_llStopTime;

		m_llStopTime = 0;
		m_timerStopped = false;
	}

	/// <summary>
	/// Stops the timer without resetting accumulated time.
	/// </summary>
	void Timer::Stop() noexcept
	{
		if (!m_timerStopped)
		{
			LARGE_INTEGER qwTime{};
			QueryPerformanceCounter(qwTime);

			m_llStopTime = qwTime.QuadPart;
			m_timerStopped = true;
		}
	}

	/// <summary>
	/// Returns elapsed time (in seconds) since last reset.
	/// </summary>
	void Timer::GetTimeValue(
		double& time
	) noexcept
	{
		const LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

		// Convert ticks → seconds
		time = (qwTime.QuadPart - m_llBaseTime)
			/ static_cast<double>(m_llQPFTicksPerSec);
	}

	/// <summary>
	/// Returns the current performance counter value.
	/// If the timer is stopped, returns the moment it was stopped.
	/// </summary>
	LARGE_INTEGER Timer::GetAdjustedCurrentTime() const
	{
		LARGE_INTEGER qwTime{};
		if (m_llStopTime != 0)
			qwTime.QuadPart = m_llStopTime;
		else
			QueryPerformanceCounter(qwTime);

		return qwTime;
	}

	/// <summary>
	/// Wrapper for QueryPerformanceCounter with error handling.
	/// </summary>
	void Timer::QueryPerformanceCounter(
		LARGE_INTEGER& lpPerformanceCount
	) const
	{
		if (::QueryPerformanceCounter(&lpPerformanceCount) == FALSE)
			THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"Sys Timer");
	}

	/*
	// -------------------------------------------------------------------------
	// Optional CPU-affinity logic (disabled)
	//
	// This can be used to prevent time drift on systems where threads migrate
	// between cores with unsynchronized TSCs.
	//
	// Left commented intentionally – modern systems usually do not need this.
	// -------------------------------------------------------------------------
	void Timer::LimitThreadAffinityToCurrentProc() noexcept
	{
		...
	}
	*/

	/// <summary>
	/// Global timer instance used by the engine.
	/// </summary>
	Timer g_timer;
}