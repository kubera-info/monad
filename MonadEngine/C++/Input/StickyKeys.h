// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Del.h"

namespace Monad::Input
{
	/// <summary>
	/// Manages the system Sticky Keys feature during application runtime.
	/// </summary>
	struct StickyKeys
	{
		/// <summary>
		/// Initializes Sticky Keys handling.
		/// </summary>
		StickyKeys() noexcept;

		/// <summary>
		/// Restores the original Sticky Keys state.
		/// </summary>
		~StickyKeys();

		OPER_DEL_NO_DEF_CTOR(StickyKeys);

	private:
		/// <summary>
		/// Enables or disables shortcut key behavior.
		/// </summary>
		/// <param name="allowKeys">True to allow shortcut keys; false to disable them.</param>
		void AllowShortcutKeys(
			bool allowKeys
		) noexcept;
	};
}
