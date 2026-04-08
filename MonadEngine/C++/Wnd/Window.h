// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <expected>
// Monad
#include "I18N/I18N.h"
#include "Input/Cursor.h"
#include "Kernel/Del.h"

namespace Monad::Wnd
{
	// =========================================================================
	// WindowClass
	// =========================================================================

	/// <summary>
	/// Base class responsible for:
	///  - registering a Win32 window class (in ctor)
	///  - unregistering it on destruction
	///
	/// Designed as a RAII helper for window-class lifetime management.
	/// </summary>
	interface WindowClass abstract
	{
		/// <summary>
		/// Registers a window class using provided name and window procedure.
		/// </summary>
		/// <param name="className">Name of the window class</param>
		/// <param name="wndProc">Window procedure callback</param>
		WindowClass(
			const wchar_t* className,
			WNDPROC wndProc
		);

		/// <summary>
		/// Unregisters previously registered window class.
		/// </summary>
		virtual ~WindowClass();

		OPER_DEL(WindowClass);

	protected:
		/// <summary>
		/// Creates a window instance based on this class.
		/// Wraps CreateWindowEx and throws on failure.
		/// </summary>
		/// <param name="dwExStyle">Extended window style</param>
		/// <param name="wndRect">Window position and size</param>
		/// <param name="lpParameter">CreateWindowEx user parameter</param>
		/// <param name="dwStyle">Window style</param>
		/// <param name="appTitle">Localized window title</param>
		/// <returns>Valid HWND or throws exception</returns>
		HWND WindowFactoryGeneric(
			const DWORD dwExStyle,
			const RECT& wndRect,
			LPVOID const lpParameter,
			const DWORD dwStyle,
			const I18N::I18NString& appTitle
		) const;

	private:
		/// <summary>
		/// Stored class name used for registration/unregistration.
		/// </summary>
		const std::wstring c_className;
	};

	/// <summary>
	/// Updates window title using application-wide localized name.
	/// </summary>
	void SetWindowTitle(
		HWND pHWnd
	) noexcept;
}