// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

#include <mutex>
#include <condition_variable>

namespace Monad::Kernel
{
	/// <summary>
	/// Synchronization helper for multithreaded systems
	/// (e.g., thread pools or worker queues).
	///
	/// Wraps a mutex and condition variable into a single utility object.
	/// </summary>
	struct Ev final
	{
		/// <summary>
		/// Wakes one waiting thread.
		/// </summary>
		inline void notify_one() noexcept
		{
			m_condVar.notify_one();
		}

		/// <summary>
		/// Wakes all waiting threads.
		/// </summary>
		inline void notify_all() noexcept
		{
			m_condVar.notify_all();
		}

		/// <summary>
		/// Provides access to the internal mutex.
		/// </summary>
		operator std::mutex& () noexcept
		{
			return m_lockTransact;
		}

		/// <summary>
		/// Locks the mutex and waits until the predicate evaluates to true.
		/// </summary>
		/// <typeparam name="P">Predicate type.</typeparam>
		/// <param name="pred">Condition to wait for.</param>
		/// <returns>A locked unique_lock guarding the mutex.</returns>
		template<class P>
		std::unique_lock<std::mutex> LockingWait(P pred)
		{
			std::unique_lock<std::mutex> lock(m_lockTransact);
			m_condVar.wait(lock, pred);
			return lock;
		}

	private:
		std::mutex m_lockTransact;
		std::condition_variable m_condVar;
	};
}
