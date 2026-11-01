// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"

// STD
#include <mutex>
#include <queue>
#include <ranges>

// Monad
#include "Core/Win32ApplicationGeneric.h"
#include "Exceptions/Exceptions.h"
#include "Kernel/ComManager.h"
#include "Thread.h"
#include "Kernel/SingleEvent.h"

namespace Monad::Threads
{
	struct ThreadPool;
	struct ThreadWndMessage;
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
	struct ThreadManager abstract
	{
		/// <summary>
		/// Abstract interface representing a single executable task.
		/// The task may be self-managed or externally managed.
		/// If constructed with ifDelete = true,
		/// the task will be destroyed automatically after execution.
		/// </summary>
		interface ITask abstract : Kernel::RefCount
		{
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
		using PtrTask = Microsoft::WRL::ComPtr<ITask>;
		using InitializerListTasks = std::initializer_list<ITask*>;

		template<typename T>
		auto PrepareTasks(T tasks, auto& safeUpdate)
		{
			auto notNullTasks =
				std::ranges::views::filter(
					tasks,
					[](auto task) { return task != nullptr; }
				);
			if (notNullTasks.begin() != notNullTasks.end())
			{
				const std::unique_lock ul(safeUpdate);
				m_taskQueue.push_range(notNullTasks);
			}
			return notNullTasks;
		}

	protected:
		/// <summary>
		/// Internal task queue.
		/// </summary>
		struct QueueTask : std::queue<ITask*>
		{
			/// <summary>
			/// Clears the queue and deletes all tasks.
			/// </summary>
			~QueueTask() noexcept;
		} m_taskQueue;
	};

	struct ThreadPool final : ThreadManager
	{
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

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete; 

		/// <summary>
		/// Adds tasks to the execution queue.
		/// - Null tasks are ignored.
		/// - Tasks are queued atomically.
		/// - Each inserted task wakes a worker thread.
		/// </summary>
		/// <param name="tasks">Collection of task pointers</param>
		template<typename T = InitializerListTasks>
		void InsertTasks(
			T tasks
		)
		{
			auto notNullTasks = PrepareTasks(tasks, m_ev.operator std::mutex &());

			// Wake up workers for each inserted task
			for ([[maybe_unused]] auto task : notNullTasks)
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

			ThreadPool* const m_threadPool;
		};

		/// <summary>
		/// Synchronization primitive used to signal worker threads.
		/// </summary>
		Kernel::Ev m_ev;

		/// <summary>
		/// Collection of worker threads.
		/// </summary>
		struct VectorThreads : std::vector<std::unique_ptr<ThreadWorker>>
		{
			using std::vector<std::unique_ptr<ThreadWorker>>::vector;
			void request_stops() noexcept;
		} m_threadPool;

	public:
		using ListPtrsTasks = std::forward_list<PtrTask>;
	};

	struct ThreadWndMessage final : ThreadManager
	{
		/// <summary>
		/// Adds tasks to the execution queue.
		/// - Null tasks are ignored.
		/// - Tasks are queued atomically.
		/// - Each inserted task should be dequed oustide the DirectX processing.
		/// </summary>
		/// <param name="tasks">Collection of task pointers</param>
		template<typename T = std::initializer_list<ITask*>>
		void InsertTasks(
			const T& tasks
		)
		{
			auto notNullTasks = PrepareTasks(tasks, m_lockMessageQueue);

			if (FALSE == Win32ApplicationGeneric::PostMessageW(WM_APP_MESSAGE_QUE))
				THROW_EXC(Monad::Exceptions::InvalidClass, E_OUTOFMEMORY, L"Failed to post message to main thread");
		}
		void InsertTask(
			ITask* task
		);
		void DequeueAndRunTask();

	private:
		ATG::CriticalSectionLockable m_lockMessageQueue;
	};
}