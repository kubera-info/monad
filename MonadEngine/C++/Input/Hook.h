// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <stdint.h>
// Monad
#include "Kernel/Del.h"

namespace Monad::Input
{
	/// <summary>
	/// RAII wrapper managing the lifetime of a Windows hook
	/// (e.g. mouse, keyboard, or other low-level hooks).
	/// </summary>
	interface Hook abstract
	{
	protected:
		/// <summary>
		/// Creates and registers a Windows hook.
		/// </summary>
		/// <param name="hookHandle">Reference to the hook handle.</param>
		/// <param name="hookID">Windows hook type identifier.</param>
		/// <param name="procedure">Hook callback procedure.</param>
		Hook(
			HHOOK & hookHandle,
			int32_t hookID,
			HOOKPROC procedure
		);

		/// <summary>
		/// Unregisters the hook and releases associated resources.
		/// </summary>
		~Hook();

		OPER_DEL(Hook);

	private:
		/// <summary>
		/// Reference to the active hook handle.
		/// </summary>
		HHOOK& m_hookHandle;
	};
}
