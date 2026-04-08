// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "System/UniqueInstance.h"

namespace Monad::System
{
	/// <summary>
	/// Creates a named mutex used to detect whether another instance
	/// of the application is already running.
	///
	/// If the mutex already exists or access is denied,
	/// the instance is treated as non-unique.
	/// </summary>
	SmartHandleAmIUnique::SmartHandleAmIUnique() noexcept :
		SmartHandle(
			CreateMutexEx(
				nullptr,                      // default security
				L"Info.Kubera.App.DirectX",   // unique global mutex name
				0,                            // no special flags
				0                             // default access
			)
		)
	{
		// If the mutex already exists or cannot be accessed,
		// this means another instance is already running.
		if (const DWORD res = GetLastError();
			ERROR_ALREADY_EXISTS == res || ERROR_ACCESS_DENIED == res)
		{
			// Explicitly release and invalidate the handle.
			// This signals "not unique" to the caller.
			Close();
			m_handle = nullptr;
		}
	}
}