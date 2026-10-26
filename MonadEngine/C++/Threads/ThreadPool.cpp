// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"

// Monad
#include "ThreadPool.h"
#include "Core/Win32ApplicationGeneric.h"

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
		ThreadManager::ITask::ITask(
			bool ifDelete
		) noexcept :
			c_ifDelete{ ifDelete }
		{}

		/// <summary>
		/// Deletes the task if it was created with ownership enabled.
		/// Used after task execution.
		/// </summary>
		void ThreadManager::ITask::Delete() noexcept
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
			m_threads.request_stops();
			// Wake all sleeping workers.
			m_ev.notify_all();
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
				ITask* task = nullptr;

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
					if (m_taskQueue.empty())
						continue;

					task = m_taskQueue.front();
					m_taskQueue.pop();
				}

				// Execute task safely
				try
				{
					task->Run();
				}
				MONAD_CATCH;

				// Cleanup if task owns itself
				task->Delete();
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
			return !m_taskQueue.empty() || stopToken.stop_requested();
		}

		/// <summary>
		/// Clears all pending commands from queue.
		/// Tasks are properly released.
		/// </summary>
		ThreadPool::QueueTask::~QueueTask() noexcept
		{
			while (!empty())
			{
				const auto task = front();
				pop();
				task->Delete();
			}
		}

		void ThreadPool::VectorThreads::request_stops() noexcept
		{
			for (auto& thread : *this)
				thread->request_stop();
		}
#pragma endregion

#pragma region ThreadWndMessage
		void ThreadWndMessage::InsertTask(
			ITask* task
		)
		{
			{
				const std::lock_guard lg(m_lockMessageQueue);
				m_taskQueue.push(task);
			}
			if(FALSE == Win32ApplicationGeneric::PostMessageW(WM_APP_MESSAGE_QUE))
				THROW_EXC(Monad::Exceptions::InvalidClass, E_OUTOFMEMORY, L"Failed to post message to main thread");
		}

		void ThreadWndMessage::DequeueAndRunTask()
		{
			const std::lock_guard lg(m_lockMessageQueue);
			while (!m_taskQueue.empty())
			{
				const auto task = m_taskQueue.front();
				m_taskQueue.pop();
				try
				{
					task->Run();
				}
				MONAD_CATCH;
				task->Delete();
			}
		}
#pragma endregion
	}
}