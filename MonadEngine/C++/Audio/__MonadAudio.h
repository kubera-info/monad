// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <xaudio2.h>
// STD
#include <string>

namespace Monad::Audio
{
	/// <summary>
	/// Dedicated queue names for voices intended for parallel playback.
	/// </summary>
	extern const std::string
		STREAM,
		DESCRIBE,
		JINGLE;
	using InitializerListQueues = std::initializer_list<std::string>;
	/// <summary>
	/// Responsible for destroying or releasing virtual audio devices.
	/// </summary>
	using FnDestroyVoice = void(*)(IXAudio2Voice* const) noexcept;

	constexpr float
		VOLUME_MUTED = 0.0f,
		VOLUME_SILENT = 0.125f,
		VOLUME_MAX = 1.0f;

	extern FnDestroyVoice DestroyVoice;
	extern FnDestroyVoice DestroySourceVoice;
}

constexpr std::string operator""_cue(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}
