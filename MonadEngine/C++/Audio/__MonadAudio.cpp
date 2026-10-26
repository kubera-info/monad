// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadAudio.h"

namespace Monad::Audio
{
	const Queue
		STREAM = "stream"_queue,
		DESCRIBE = "describe"_queue,
		JINGLE = "jingle"_queue;

	FnDestroyVoice DestroyVoice = [](IXAudio2Voice* const voice) noexcept
		{
			if (voice)
				voice->DestroyVoice();
		};
	FnDestroyVoice DestroySourceVoice = [](IXAudio2Voice* const voice) noexcept
		{
			if (voice)
			{
				static_cast<IXAudio2SourceVoice*> (voice)->Stop();
				voice->DestroyVoice();
			}
		};
}

Cue operator""_cue(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

Queue operator""_queue(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}