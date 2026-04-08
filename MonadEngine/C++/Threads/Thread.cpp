// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Thread.h"
// Monad
#include "Exceptions/Exceptions.h"

using namespace std;

namespace Monad
{
	using namespace Exceptions;

	namespace Threads
	{
		namespace
		{
			/// <summary>
			/// Internal wrapper used as a thread entry point.
			/// Responsible for:
			///  - initializing COM for the thread
			///  - invoking the actual thread routine
			///  - catching all framework-level exceptions
			/// </summary>
			inline void Call(
				Thread& thread
			) noexcept
			{
				try
				{
					// Initialize COM for this worker thread
					Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
					THROW_EXC_IFFAILED_RO(initialize);

					// Execute user-defined thread routine
					thread.Routine();
				}
				MONAD_CATCH;
			}
		}

		/// <summary>
		/// Creates a thread object and immediately starts execution.
		/// </summary>
		Thread::Thread(
			const wstring_view name
		) noexcept :
			m_thread(&Thread::HardwareExceptionStub, this)
		{
			SetMetaData(name);
		}

		/// <summary>
		/// Assigns metadata to the running thread:
		///  - thread name (visible in debugger)
		///  - stop_token reference
		/// </summary>
		void Thread::SetMetaData(
			const wstring_view name
		) noexcept
		{
			assert(!name.empty() && "ThreadName");
			assert(joinable() && "joinable");

			m_stopToken = m_thread.get_stop_token();

			// Windows-visible thread name
			SetThreadDescription(
				m_thread.native_handle(),
				name.data()
			);
		}

		/// <summary>
		/// Joins the thread safely.
		/// Exceptions are intentionally swallowed.
		/// </summary>
		void Thread::join() noexcept
		{
			try
			{
				m_thread.join();
			}
			catch (invalid_argument&)
			{
				// join() may throw if thread is not joinable
				// ignored intentionally
			}
		}

		/// <summary>
		/// Returns whether the underlying std::jthread is joinable.
		/// </summary>
		bool Thread::joinable() const noexcept
		{
			return m_thread.joinable();
		}

		/// <summary>
		/// Requests cooperative stop for the thread.
		/// </summary>
		void Thread::request_stop() noexcept
		{
			m_thread.request_stop();
		}

		/// <summary>
		/// Thread entry wrapper with optional SEH protection.
		/// Used to guard against hardware exceptions.
		/// </summary>
		void Thread::HardwareExceptionStub() noexcept
		{
#if MONAD_HARDWARE_EXCEPTIONS == 1
			__try
			{
#endif
				Call(*this);
#if MONAD_HARDWARE_EXCEPTIONS == 1
			}
			__except (filter(GetExceptionCode(), GetExceptionInformation()))
			{
				// Hardware exception caught and suppressed
			}
#endif
		}

		/// <summary>
		/// Starts the thread if it is not already running.
		/// </summary>
		void Thread::RunThread(
			const wstring_view name
		)
		{
			assert(!joinable() && "Thread is already running");

			m_thread = jthread(&Thread::HardwareExceptionStub, this);
			SetMetaData(name);
		}
	}
}