// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "__MonadSystem.h"

namespace Monad::System
{
	/// <summary>
	/// Helper class used to check whether the current process instance
	/// is the only running instance of the application.
	///
	/// Internally it relies on a named Windows mutex.
	/// If the mutex already exists or access is denied, the application
	/// is assumed to be already running.
	///
	/// This class does NOT terminate the process automatically —
	/// it only provides information via its validity state.
	/// </summary>
	struct SmartHandleAmIUnique final : SmartHandle
	{
		/// <summary>
		/// Creates a named mutex and checks whether it already exists.
		///
		/// If:
		/// - mutex is newly created → this is the first instance
		/// - mutex already exists or access is denied → another instance is running
		///
		/// In the second case, the internal handle is released and set to nullptr.
		/// </summary>
		SmartHandleAmIUnique() noexcept;
	};
}