// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AudioThread.h"
// Monad
#include "Persistent.h"

namespace Monad::Audio
{
	TaskAudioGeneric::TaskAudioGeneric() noexcept :
		ITask{ false }
	{}

	void TaskRestart::Run()
	{
		g_persistentAudio->InternalRestart();
	}

	void TaskShutdown::Run()
	{
		g_persistentAudio->Shutdown();
	}

	void TaskStartStream::Run()
	{
		g_persistentAudio->InternalStartStream();
	}
}