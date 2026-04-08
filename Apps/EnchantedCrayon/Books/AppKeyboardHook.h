// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Input/Hook.h"

namespace Monad::App
{
	/// <summary>
	/// Keyboard hook used to block the Windows logo key.
	/// Prevents the operating system from interrupting
	/// the application (e.g. opening the Start Menu).
	/// </summary>
	struct KeyboardHookNoWinKey : private Input::Hook
	{
		/// <summary>
		/// Installs a low-level keyboard hook that filters
		/// Windows key events (LWIN / RWIN).
		/// </summary>
		KeyboardHookNoWinKey();
	};
}