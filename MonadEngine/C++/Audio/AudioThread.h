// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Threads/ThreadPool.h"

namespace Monad::Audio
{
	struct PersistentAudio;

	/// <summary>
	/// An audio task intended for execution within a thread pool, maintaining a reference to a PersistentAudio object.
	/// </summary>
	interface TaskAudioGeneric abstract : Threads::ThreadPool::ITask
	{
		TaskAudioGeneric() noexcept;
	};

#if defined TASK_AUDIO
#	error TASK_AUDIO already defined
#endif

	/// <summary>
	/// Base interface for audio thread pool tasks.
	/// </summary>
#define TASK_AUDIO(NAME)\
	struct NAME final : TaskAudioGeneric\
	{\
	static_assert(std::string_view(#NAME).starts_with("Task"), "NAME must be begin with 'Task' prefix");\
	using TaskAudioGeneric::TaskAudioGeneric;\
	private:\
		void Run() final;\
	};

	/// <summary>
	/// Stops and reinitializes the audio engine, with optional hardware reassignment.
	/// </summary>
	TASK_AUDIO(TaskRestart);

	/// <summary>
	/// Deinitializes the audio engine.
	/// </summary>
	TASK_AUDIO(TaskShutdown);

	/// <summary>
	/// Starts playing a randomly selected melody as an MP3 stream.
	/// </summary>
	TASK_AUDIO(TaskStartStream);

#undef TASK_AUDIO
}
