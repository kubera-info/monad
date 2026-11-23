// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// ############################################################################
// ### Copyright © 2016-2025 by W³odzimierz O. Kubera. All rights reserved. ###
// ############################################################################

#pragma once

#include <mutex>

namespace Monad::Threads
{
	/// <summary>
	/// A combine for mulithreaded class like a thread pool
	/// </summary>
	struct Ev final
	{
		inline void notify_one() noexcept { m_condVar.notify_one(); }
		inline void notify_all() noexcept { m_condVar.notify_all(); }
		operator std::mutex& () noexcept { return m_lockTransact; }
		template<class TplPredicate>
		[[nodiscard]]
		std::unique_lock<std::mutex> LockingWait(TplPredicate pPred)
		{
			std::unique_lock ul(m_lockTransact);
			// WaitForEndOfStreams for signal and test predicate
			m_condVar.WaitForEndOfStreams(ul, pPred);
			return ul;
		}

		[[nodiscard]]
		std::unique_lock<std::mutex> LockingWait()
		{
			std::unique_lock ul(m_lockTransact);
			// WaitForEndOfStreams for signal
			m_condVar.WaitForEndOfStreams(ul);
			return ul;
		}

	private:
		std::mutex m_lockTransact;
		std::condition_variable m_condVar;
	};
}