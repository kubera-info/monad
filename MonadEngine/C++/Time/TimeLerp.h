// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Events.h"

namespace Monad::Time
{
	/// <summary>
	/// TimeLerp
	/// 
	/// A time–based linear interpolator.
	/// Moves its internal value from startValue to stopValue
	/// during a specified time interval.
	///
	/// The object automatically registers itself in the frame-update system
	/// when started and unregisters itself when finished.
	///
	/// Typical usage:
	/// - animations
	/// - transitions
	/// - fades
	/// - smooth value changes
	/// </summary>
	struct TimeLerp : Kernel::EventFrameMoveStub
	{
		/// <summary>
		/// Creates a new interpolation object.
		/// </summary>
		/// <param name="durationInSeconds">Duration of interpolation</param>
		/// <param name="startValue">Initial value</param>
		/// <param name="stopValue">Final value</param>
		TimeLerp(
			const double durationInSeconds = 1.0,
			const double startValue = 0.0,
			const double stopValue = 1.0
		) noexcept;

		/// <summary>
		/// Changes only the starting value.
		/// </summary>
		void SetStartValue(
			const double startValue
		) noexcept;

		/// <summary>
		/// Returns the initial value of the interpolation.
		/// </summary>
		double GetStartValue() const noexcept;

		/// <summary>
		/// Returns the final value of the interpolation.
		/// </summary>
		double GetStopValue() const noexcept;

		/// <summary>
		/// Returns the current interpolated value.
		/// </summary>
		float GetCurrentValue() const noexcept;

		/// <summary>
		/// Returns true if interpolation has completed.
		/// </summary>
		bool IsFinished() const noexcept;

		/// <summary>
		/// Returns true if interpolation is currently running.
		/// </summary>
		bool IsRunning() const noexcept;

		/// <summary>
		/// Starts interpolation using current parameters.
		/// Automatically registers the object in the frame update system.
		/// </summary>
		void Start();

		/// <summary>
		/// Sets new parameters and starts interpolation.
		/// </summary>
		void StartWithParameters(
			const double durationInSeconds,
			const double startValue,
			const double stopValue
		);

		/// <summary>
		/// Sets new start and stop values and starts interpolation.
		/// Duration remains unchanged.
		/// </summary>
		void StartWithParameters(
			const double startValue,
			const double stopValue
		);

		/// <summary>
		/// Immediately finishes interpolation and calls OnFlush().
		/// </summary>
		void Flush();

	protected:
		/// <summary>
		/// Called when Flush() is executed.
		/// </summary>
		virtual void OnFlush() {}

		/// <summary>
		/// Called when interpolation starts.
		/// </summary>
		virtual void OnStart() {}

		/// <summary>
		/// Called when interpolation finishes naturally.
		/// </summary>
		virtual void OnStop() {}

		/// <summary>
		/// Called every frame while interpolation is active.
		/// </summary>
		virtual void OnFrameMove() {}

	private:
		/// <summary>
		/// Internal frame callback registered in the event system.
		/// </summary>
		bool OnFrameMoveStub() final;

		/// <summary>
		/// Computes the interpolated value in double precision.
		/// </summary>
		inline double GetCurrentValueInternal() const noexcept;

		/// <summary>
		/// Sets full interpolation parameters and resets state.
		/// </summary>
		void SetLerpParameters(
			const double durationInSeconds,
			const double startValue,
			const double stopValue
		) noexcept;

		/// <summary>
		/// Sets start and stop values only.
		/// </summary>
		void SetLerpParameters(
			const double startValue,
			const double stopValue
		) noexcept;

		/// <summary>
		/// Resets interpolation state to "not started".
		/// </summary>
		inline void Reset() noexcept;

		double m_startTime;
		double m_durationInSeconds;
		double m_startValue;
		double m_stopValue;
	};
}