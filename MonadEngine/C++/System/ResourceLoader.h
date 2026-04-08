// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Threads/ThreadPool.h"

namespace Monad::System
{
	struct TaskSaverProfileIco : public Threads::ThreadPool::ITask
	{
		TaskSaverProfileIco();
		void Run() final;
	} extern g_SetProfileFolder;
}