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
		if (g_persistentAudio && !empty() /* && g_persistentAudio*/)
			g_persistentAudio->PlayJingle(Cue{ GetRandomString() });
	}
}