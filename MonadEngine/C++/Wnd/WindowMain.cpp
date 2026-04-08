// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "WindowMain.h"
// Monad
#include "Globals/__MonadGlobals.h"

namespace Monad::Wnd
{
	/// <summary>
	/// Brings the main Direct3D window to the foreground and makes it active.
	/// 
	/// This function is used after window creation or when restoring focus
	/// (e.g. after Alt+Tab, monitor changes, or switching fullscreen mode).
	/// 
	/// The sequence of calls follows typical Win32 behavior for reliably
	/// activating a window in modern Windows versions.
	/// </summary>
	void ActivateDirect3DWindow(
		const HWND hWnd
	) noexcept
	{
		// Brings the window to the top of the Z-order
		BringWindowToTop(hWnd);

		// Attempts to give the window foreground activation
		SetForegroundWindow(hWnd);

		// Marks the window as the active one for the calling thread
		SetActiveWindow(hWnd);

		// Intentionally not used:
		// - SetFocus()       → may fail depending on foreground lock rules
		// - WM_SETCURSOR     → cursor handled elsewhere (Input subsystem)
	}

	/// <summary>
	/// Constructs the main application window class.
	/// 
	/// This registers the window class under a fixed name ("WndClassMain")
	/// and binds it with the provided window procedure.
	/// 
	/// The actual window is NOT created here — only the class definition.
	/// </summary>
	WindowClassMain::WindowClassMain(WNDPROC wndProc) :
		WindowClass{ L"WndClassMain", wndProc }
	{}

	/// <summary>
	/// Creates the main application window.
	/// 
	/// The function:
	/// - creates the HWND using the previously registered class,
	/// - stores it in the global application state,
	/// - returns the window handle to the caller.
	/// 
	/// The window is created in a standard overlapped style and is intended
	/// to be used as the main Direct3D rendering surface.
	/// </summary>
	HWND WindowClassMain::WindowFactory(
		const RECT& wndRect,
		const I18N::I18NString& appTitle
	) const
	{
		// Create the main window and store it globally
		Globals::g_mainWindow = WindowFactoryGeneric(
			WS_EX_APPWINDOW,
			wndRect,
			nullptr,
			WS_OVERLAPPEDWINDOW,
			appTitle
		);

		return Globals::g_mainWindow;
	}
}