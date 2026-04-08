// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "TimeLerp.h"
// STD
#include <assert.h>
#include <cmath>
// Monad
#include "__MonadTime.h"

namespace Monad::Time
{
#pragma region TimeLerp

	namespace
	{
		/// <summary>
		/// Special values describing the internal state of the interpolation.
		/// START_STATE  – interpolation has not been started yet
		/// STOP_STATE   – interpolation has finished
		/// </summary>
		constexpr double START_STATE(0.0), STOP_STATE(-1.0);
	}

	/// <summary>
	/// Creates a new time-based linear interpolation.
	/// </summary>
	/// <param name="durationInSeconds">Total interpolation time</param>
	/// <param name="startValue">Initial value</param>
	/// <param name="stopValue">Final value</param>
	TimeLerp::TimeLerp(
		const double durationInSeconds,
		const double startValue,
		const double stopValue
	) noexcept :
		m_startTime{ START_STATE },
		m_durationInSeconds{ durationInSeconds },
		m_startValue{ startValue },
		m_stopValue{ stopValue }
	{
		assert(0.0 < m_durationInSeconds);
	}

	/// <summary>
	/// Sets new interpolation parameters and resets the state.
	/// </summary>
	void TimeLerp::SetLerpParameters(
		const double durationInSeconds,
		const double startValue,
		const double stopValue
	) noexcept
	{
		SetLerpParameters(startValue, stopValue);
		assert(0.0 < m_durationInSeconds);
		m_durationInSeconds = durationInSeconds;
	}

	/// <summary>
	/// Sets only start and end values, keeping current duration.
	/// </summary>
	inline void TimeLerp::SetLerpParameters(
		const double startValue,
		const double stopValue
	) noexcept
	{
		Reset();
		m_startValue = startValue;
		m_stopValue = stopValue;
	}

	/// <summary>
	/// Changes only the starting value.
	/// </summary>
	void TimeLerp::SetStartValue(
		const double startValue
	) noexcept
	{
		m_startValue = startValue;
	}

	/// <summary>
	/// Resets the interpolator to an unstarted state.
	/// </summary>
	inline void TimeLerp::Reset() noexcept
	{
		m_startTime = START_STATE;
	}

	/// <summary>
	/// Forces the interpolation to finish immediately.
	/// </summary>
	void TimeLerp::Flush()
	{
		m_startTime = STOP_STATE;
		OnFlush();
	}

	/// <summary>
	/// Starts interpolation from the current global time.
	/// </summary>
	void TimeLerp::Start()
	{
		m_startTime = g_fTime;
		Register();
		OnStart();
	}

	double TimeLerp::GetStartValue() const noexcept
	{
		return m_startValue;
	}

	double TimeLerp::GetStopValue() const noexcept
	{
		return m_stopValue;
	}

	/// <summary>
	/// Sets parameters and immediately starts interpolation.
	/// </summary>
	void TimeLerp::StartWithParameters(
		const double durationInSeconds,
		const double startValue,
		const double stopValue
	)
	{
		SetLerpParameters(durationInSeconds, startValue, stopValue);
		Start();
	}

	/// <summary>
	/// Starts interpolation using existing duration.
	/// </summary>
	void TimeLerp::StartWithParameters(
		const double startValue,
		const double stopValue
	)
	{
		SetLerpParameters(startValue, stopValue);
		Start();
	}

	/// <summary>
	/// Called every frame by the event system.
	/// Controls lifecycle and auto-stop behavior.
	/// </summary>
	bool TimeLerp::OnFrameMoveStub()
	{
		if (GetCurrentValue() == m_stopValue && STOP_STATE != m_startTime)
		{
			Flush();
			Unregister();
			OnStop();
		}

		OnFrameMove();
		return true;
	}

	/// <summary>
	/// Computes the interpolated value without type conversion.
	/// </summary>
	inline double TimeLerp::GetCurrentValueInternal() const noexcept
	{
		if (START_STATE == m_startTime)
			return m_startValue;

		if (STOP_STATE == m_startTime)
			return m_stopValue;

		if (m_startTime + m_durationInSeconds > g_fTime)
			return std::lerp(
				m_startValue,
				m_stopValue,
				(g_fTime - m_startTime) / m_durationInSeconds
			);

		return m_stopValue;
	}

	/// <summary>
	/// Returns current interpolated value as float.
	/// </summary>
	float TimeLerp::GetCurrentValue() const noexcept
	{
		return static_cast<float>(GetCurrentValueInternal());
	}

	/// <summary>
	/// Returns true if interpolation has finished.
	/// </summary>
	bool TimeLerp::IsFinished() const noexcept
	{
		return STOP_STATE == m_startTime;
	}

	/// <summary>
	/// Returns true if interpolation is currently active.
	/// </summary>
	bool TimeLerp::IsRunning() const noexcept
	{
		return START_STATE != m_startTime && !IsFinished();
	}

#pragma endregion TimeLerp
}