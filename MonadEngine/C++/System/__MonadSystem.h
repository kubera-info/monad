// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// Monad
#include "Kernel/Del.h"

namespace Monad::System
{
	/// <summary>
	/// Lightweight RAII wrapper for Windows HANDLE.
	///
	/// Purpose:
	///  • Owns a Windows HANDLE
	///  • Ensures proper release via CloseHandle()
	///  • Designed mainly for global / system objects (mutexes, semaphores, etc.)
	/// </summary>
	struct SmartHandle
	{
		/// <summary>
		/// Constructs wrapper from an existing HANDLE.
		/// </summary>
		/// <param name="handle">Native Windows handle</param>
		SmartHandle(
			HANDLE handle
		) noexcept;

		/// <summary>
		/// Releases the underlying handle (if any).
		/// </summary>
		virtual ~SmartHandle();

		OPER_DEL_NO_DEF_CTOR(SmartHandle);

		/// <summary>
		/// Implicit conversion to raw HANDLE.
		/// </summary>
		/// <returns>
		/// Native HANDLE value stored inside this object.
		/// </returns>
		operator const HANDLE() const noexcept;

		/// <summary>
		/// Checks whether handle is valid.
		/// </summary>
		/// <returns>
		/// true  – handle is non-null  
		/// false – handle is null
		/// </returns>
		operator bool() const noexcept;

		/// <summary>
		/// Releases the stored handle using CloseHandle().
		/// Safe to call multiple times.
		/// </summary>
		void Close() noexcept;

		HANDLE Detach() noexcept;

	protected:
		/// <summary>
		/// Stored Windows handle.
		/// </summary>
		HANDLE m_handle;
	};
}