// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadAudio.h"

namespace Monad::Audio
{
	const std::string
		STREAM = "stream",
		DESCRIBE = "desrcibe",
		JINGLE = "jingle";

	FnDestroyVoice DestroyVoice = [](IXAudio2Voice* const voice) noexcept
		{
			assert(voice);
			voice->DestroyVoice();
		};
}