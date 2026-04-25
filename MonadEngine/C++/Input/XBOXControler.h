// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Threads/Thread.h"

struct DXSample;
namespace Monad::Input
{
	/// <summary>
	/// Represents input data from an Xbox controller, handled similarly to other pointing devices.
	/// </summary>
	struct ThreadXboxController final : Threads::Thread
	{
		ThreadXboxController() noexcept;
		~ThreadXboxController() = default;
		OPER_DEL_NO_DEF_CTOR(ThreadXboxController);

	private:
		void Routine() final;
	};
}
