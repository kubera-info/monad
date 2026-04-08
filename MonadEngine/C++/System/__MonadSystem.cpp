// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadSystem.h"

namespace Monad::System
{
	/// <summary>
	/// Lightweight RAII wrapper for Windows HANDLE.
	/// Automatically closes the handle when leaving scope.
	/// </summary>
	SmartHandle::SmartHandle(
		HANDLE handle
	) noexcept :
		m_handle(handle)
	{}

	/// <summary>
	/// Destructor automatically releases the owned HANDLE.
	/// Safe to call even if handle is null.
	/// </summary>
	SmartHandle::~SmartHandle()
	{
		Close();
	}

	/// <summary>
	/// Implicit conversion to raw HANDLE.
	/// Useful for WinAPI calls expecting HANDLE.
	/// </summary>
	SmartHandle::operator const HANDLE() const noexcept
	{
		return m_handle;
	}

	/// <summary>
	/// Checks whether the handle is valid (non-null).
	/// </summary>
	SmartHandle::operator bool() const noexcept
	{
		return nullptr != m_handle;
	}

	/// <summary>
	/// Releases the underlying handle if valid.
	/// Safe to call multiple times.
	/// </summary>
	void SmartHandle::Close() noexcept
	{
		if (operator bool())
			::CloseHandle(m_handle);
	}

	HANDLE SmartHandle::Detach() noexcept
	{
		auto handle = m_handle;
		m_handle = nullptr;
		return handle;
	}

}