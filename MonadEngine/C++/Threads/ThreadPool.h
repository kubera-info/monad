// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <mutex>
#include <queue>
#include <ranges>
// Monad
#include "Exceptions/Exceptions.h"
#include "Kernel/ComManager.h"
#include "Thread.h"
#include "Kernel/SingleEvent.h"

namespace Monad::Threads
{
	/// <summary>
	/// Thread pool implementation responsible for executing asynchronous tasks.
	/// 
	/// Characteristics:
	/// - Fixed number of worker threads
	/// - Tasks executed sequentially per worker
	/// - Cooperative shutdown using stop_token
	/// - Exception-safe execution
	/// - Optional ownership of task lifetime
	/// </summary>
	struct ThreadPool
	{
		/// <summary>
		/// Abstract interface representing a single executable task.
		/// The task may be self-managed or externally managed.
		/// If constructed with ifDelete = true,
		/// the task will be destroyed automatically after execution.
		/// </summary>
		interface ITask abstract : Kernel::RefCount
		{
			friend ThreadPool;

		/// <summary>
		/// Constructs a task object.
		/// </summary>
		/// <param name="ifDelete">
		/// If true, the task will be released automatically after execution.
		/// </param>
		ITask(
			bool ifDelete = true
		) noexcept;

		/// <summary>
		/// Main task body.
		/// This function is executed on a worker thread.
		/// May throw C++ or hardware exceptions.
		/// All exceptions are caught by the thread pool.
		/// </summary>
		virtual void Run() = 0;

		/// <summary>
		/// Deletes the task if it was created with ownership enabled.
		/// </summary>
		void Delete() noexcept;

	private:
		/// <summary>
		/// Determines whether the task should delete itself after execution.
		/// </summary>
		const bool c_ifDelete;
		};

		/// <summary>
		/// Creates a thread pool with a given number of worker threads.
		/// </summary>
		/// <param name="name">User-friendly thread name (for debugger / diagnostics)</param>
		/// <param name="countOfWorkers">Number of worker threads</param>
		ThreadPool(
			const std::wstring_view name,
			size_t countOfWorkers = 1
		);

		/// <summary>
		/// Destructor.
		/// Gracefully shuts down all worker threads and clears the queue.
		/// </summary>
		~ThreadPool();

		OPER_DEL(ThreadPool);

		/// <summary>
		/// Adds tasks to the execution queue.
		/// - Null tasks are ignored.
		/// - Tasks are queued atomically.
		/// - Each inserted task wakes a worker thread.
		/// </summary>
		/// <param name="tasks">Collection of task pointers</param>
		template<typename T = std::initializer_list<ITask*>>
		void InsertTasks(
			const T& tasks
		)
		{
			assert(tasks.begin() != tasks.end() && "At least one task must be provided");
			assert(std::ranges::all_of(tasks, [](auto task) { return task != nullptr; }) && "Null tasks are not allowed");

			{
				const std::unique_lock<std::mutex> ul(m_ev);
				m_commandQueue.push_range(tasks);
			}

			// Wake up workers for each inserted task
			for ([[maybe_unused]] auto command : tasks)
				m_ev.notify_one();
		}

		void InsertTask(
			ITask* task
		)
		{
			InsertTasks({ task });
		}

	private:
		/// <summary>
		/// Checks whether a worker should wake up.
		/// </summary>
		/// <returns>
		/// True if there are tasks to execute or stop was requested.
		/// </returns>
		inline bool HasSignal(
			std::stop_token stopToken
		) const noexcept;

		/// <summary>
		/// Worker thread main loop.
		/// Waits for tasks and executes them safely.
		/// </summary>
		inline void RoutineWrap(
			std::stop_token stopToken
		);

		/// <summary>
		/// Worker thread implementation.
		/// Each instance runs one OS thread.
		/// </summary>
		struct ThreadWorker : Thread
		{
			ThreadWorker(
				ThreadPool* const threadPool
			) noexcept;

		private:
			/// <summary>
			/// Entry point for worker execution.
			/// </summary>
			void Routine() final;

			ThreadPool* const m_threads;
		};

		/// <summary>
		/// Synchronization primitive used to signal worker threads.
		/// </summary>
		Kernel::Ev m_ev;

		/// <summary>
		/// Internal task queue.
		/// </summary>
		struct QueueCommand : std::queue<ITask*>
		{
			/// <summary>
			/// Clears the queue and deletes all tasks.
			/// </summary>
			void clear() noexcept;
		} m_commandQueue;

		/// <summary>
		/// Collection of worker threads.
		/// </summary>
		std::vector<std::unique_ptr<ThreadWorker>> m_threads;

	public:
		using PtrTask = Microsoft::WRL::ComPtr<ITask>;
		using ListPtrsTasks = std::forward_list<PtrTask>;
	};
}