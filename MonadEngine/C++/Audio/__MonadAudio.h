// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <xaudio2.h>

// STD
#include <string>
#include <vector>

// Monad
#include "Kernel/Patterns.h"

namespace Monad::Audio
{
	constexpr float
		VOLUME_MUTED = 0.0f,
		VOLUME_SILENT = 0.125f,
		VOLUME_MAX = 1.0f;

	/// <summary>
	/// Responsible for destroying audio voices.
	/// </summary>
	using FnDestroyVoice = void(*)(IXAudio2Voice* const) noexcept;
	extern FnDestroyVoice DestroyVoice;
	extern FnDestroyVoice DestroySourceVoice;
}

using Cue = std::string;
Cue operator""_cue(const char* literal, std::size_t sizeOfString);
using VectorCue = std::vector<Cue>;
using VectorRandomCue = Monad::Kernel::VectorRandom<Cue>;

using Queue = std::string;
Queue operator""_queue(const char* literal, std::size_t sizeOfString);

namespace Monad::Audio
{
	/// <summary>
	/// Dedicated queue names for voices intended for parallel playback.
	/// </summary>
	extern const Queue
		STREAM,
		DESCRIBE,
		JINGLE;
	using InitializerListQueues = std::initializer_list<Queue>;
}