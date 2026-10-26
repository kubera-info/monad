// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Jingle.h"

namespace Monad::Audio
{
	void RandomJingle::PlayJingle() const
	{
		THROW_EXC_IFNULL(Exceptions::AudioFailed, g_persistentAudio, L"Audio is not initialized");
		if (!empty() /* && g_persistentAudio*/)
			g_persistentAudio->PlayJingle(Cue{ GetRandomString() });
	}
}