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
		ThreadXboxController(const ThreadXboxController&) = delete;
		ThreadXboxController& operator=(const ThreadXboxController&) = delete;
		ThreadXboxController(ThreadXboxController&&) = delete;
		ThreadXboxController& operator=(ThreadXboxController&&) = delete;

	private:
		void Routine() final;
	};
}
