// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "ThreadPool.h"

namespace Monad
{
	using namespace Exceptions;

	namespace Threads
	{
#pragma region ITask

		/// <summary>
		/// Base interface for a task executed by ThreadPool.
		/// </summary>
		/// <param name="ifDelete">
		/// Determines whether the task should be automatically deleted
		/// after execution.
		/// </param>
		ThreadPool::ITask::ITask(
			bool ifDelete
		) noexcept :
			c_ifDelete{ ifDelete }
		{}

		/// <summary>
		/// Deletes the task if it was created with ownership enabled.
		/// Used after task execution.
		/// </summary>
		void ThreadPool::ITask::Delete() noexcept
		{
			if (c_ifDelete)
				Release();
		}

#pragma endregion

#pragma region WorkerThread

		/// <summary>
		/// Worker thread bound to a ThreadPool instance.
		/// Each worker executes tasks from the shared queue.
		/// </summary>
		ThreadPool::ThreadWorker::ThreadWorker(
			ThreadPool* const threadPool
		) noexcept :
			m_threads(threadPool)
		{}

		/// <summary>
		/// Entry point for worker thread execution.
		/// Delegates logic to ThreadPool::RoutineWrap().
		/// </summary>
		void ThreadPool::ThreadWorker::Routine()
		{
			m_threads->RoutineWrap(m_stopToken);
		}
#pragma endregion

#pragma region ThreadPool
		/// <summary>
		/// Constructs a thread pool with a given number of worker threads.
		/// Each worker is started immediately.
		/// </summary>
		ThreadPool::ThreadPool(
			const std::wstring_view name,
			size_t workersCount
		)
		{
			m_threads.reserve(workersCount);

			for (size_t i = 0; i < workersCount; ++i)
				m_threads.emplace_back(
					std::make_unique<ThreadWorker>(this)
				)->RunThread(name);
		}

		/// <summary>
		/// Gracefully shuts down the thread pool.
		///
		/// Order of shutdown:
		/// 1. Request stop for all worker threads
		/// 2. Wake them using condition variable
		/// 3. Join all threads
		/// 4. Clear remaining queued tasks
		/// </summary>
		ThreadPool::~ThreadPool()
		{
			// Request stop for all workers.
			for (auto& thread : m_threads)
				thread->request_stop();

			// Wake all sleeping workers.
			m_ev.notify_all();

			// Wait until all worker threads exit.
			for (auto& thread : m_threads)
				thread->join();

			// Safely clear remaining queued commands.
			const std::lock_guard lg(m_ev.operator std::mutex & ());
			m_commandQueue.clear();
		}

		/// <summary>
		/// Main worker loop.
		///
		/// Each worker:
		/// - waits for new commands or stop request
		/// - executes task
		/// - handles exceptions
		/// - destroys task if required
		/// </summary>
		inline void ThreadPool::RoutineWrap(
			std::stop_token stopToken
		)
		{
			while (true)
			{
				ITask* command = nullptr;

				{
					// Wait until either:
					//  - a task appears
					//  - stop is requested
					auto ul = m_ev.LockingWait(
						[this, &stopToken]() noexcept
						{
							return HasSignal(stopToken);
						}
					);

					// Exit condition
					if (stopToken.stop_requested())
						break;

					// Defensive: queue may be empty due to spurious wakeups
					if (m_commandQueue.empty())
						continue;

					command = m_commandQueue.front();
					m_commandQueue.pop();
				}

				// Execute task safely
				try
				{
					command->Run();
				}
				MONAD_CATCH;

				// Cleanup if task owns itself
				command->Delete();
			}
		}

		/// <summary>
		/// Determines whether worker should wake up.
		/// </summary>
		/// <returns>
		/// True if there is pending work or stop was requested.
		/// </returns>
		inline bool ThreadPool::HasSignal(
			std::stop_token stopToken
		) const noexcept
		{
			return !m_commandQueue.empty() || stopToken.stop_requested();
		}

		/// <summary>
		/// Clears all pending commands from queue.
		/// Tasks are properly released.
		/// </summary>
		void ThreadPool::QueueCommand::clear() noexcept
		{
			while (!empty())
			{
				const auto command = front();
				pop();
				command->Delete();
			}
		}
#pragma endregion
	}
}