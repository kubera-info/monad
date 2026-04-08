// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string_view>
#include <thread>
// Monad
#include "Kernel/Del.h"

namespace Monad::Threads
{
	/// <summary>
	/// Abstract base class for managed threads used inside Monad.
	///
	/// This class provides:
	///  - unified thread startup logic
	///  - hardware exception isolation
	///  - automatic COM initialization
	///  - controlled lifetime via std::jthread
	///
	/// The derived class must implement Routine(), which becomes
	/// the body of the thread.
	/// </summary>
	interface Thread abstract
	{
		/// <summary>
		/// Default constructor.
		/// Thread is NOT started automatically.
		/// </summary>
		Thread() noexcept = default;

		/// <summary>
		/// Constructs and immediately starts the thread.
		/// </summary>
		/// <param name="name">
		/// Human-readable name of the thread.
		/// Used by debuggers and crash reports.
		/// </param>
		Thread(
			const std::wstring_view name
		) noexcept;

		OPER_DEL_NO_DEF_CTOR(Thread);
		virtual ~Thread() = default;

		/// <summary>
		/// Main execution body of the thread.
		///
		/// This method:
		///  - runs inside a protected wrapper
		///  - may throw C++ or hardware exceptions
		///  - executes with COM initialized
		///
		/// Implemented by derived classes.
		/// </summary>
		virtual void Routine() = 0;

		/// <summary>
		/// Starts execution of the thread.
		/// Must not be called if the thread is already running.
		/// </summary>
		/// <param name="name">
		/// Logical name of the thread, visible in debugger
		/// and diagnostic tools.
		/// </param>
		void RunThread(const std::wstring_view name);

		/// <summary>
		/// Joins the thread.
		/// This method never throws, even if the thread is not joinable.
		/// </summary>
		void join() noexcept;

		/// <summary>
		/// Checks whether the underlying std::jthread is joinable.
		/// </summary>
		/// <returns>true if joinable, false otherwise</returns>
		bool joinable() const noexcept;

		/// <summary>
		/// Requests cooperative thread termination.
		/// The thread must periodically check stop_token.
		/// </summary>
		void request_stop() noexcept;

		/// <summary>
		/// Stop token associated with the thread.
		/// Accessible by derived classes for cancellation logic.
		/// </summary>
		std::stop_token m_stopToken;

	private:
		/// <summary>
		/// Assigns metadata to the running thread:
		///  - thread name
		///  - stop_token
		/// </summary>
		void SetMetaData(const std::wstring_view name) noexcept;

		/// <summary>
		/// Thread entry wrapper.
		///
		/// This method:
		///  - initializes COM
		///  - executes Routine()
		///  - optionally guards against hardware exceptions
		///
		/// It is never called directly by user code.
		/// </summary>
		void HardwareExceptionStub() noexcept;

		/// <summary>
		/// Internal std::jthread instance.
		/// Owns the actual OS thread.
		/// </summary>
		std::jthread m_thread;
	};
}