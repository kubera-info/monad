// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Window.h"

namespace Monad::Wnd
{
	/// <summary>
	/// Brings the Direct3D window to foreground and activates it.
	/// 
	/// Used after window creation and in situations where the application
	/// regains focus (startup, Alt+Tab, monitor switch, fullscreen restore).
	/// 
	/// This function wraps standard WinAPI calls responsible for
	/// correct activation order.
	/// </summary>
	void ActivateDirect3DWindow(
		const HWND hWnd
	) noexcept;

	/// <summary>
	/// Main application window class (Direct3D 12 host).
	/// 
	/// This class:
	/// - registers a Win32 window class,
	/// - creates the main rendering window,
	/// - owns no rendering logic itself,
	/// - acts as a thin wrapper over WindowClass.
	/// 
	/// The actual rendering and message handling is done elsewhere.
	/// </summary>
	struct WindowClassMain final : WindowClass
	{
		/// <summary>
		/// Registers the main application window class.
		/// </summary>
		/// <param name="wndProc">
		/// Pointer to the Win32 window procedure used by this window class.
		/// </param>
		WindowClassMain(
			WNDPROC wndProc
		);

		/// <summary>
		/// Creates the main application window.
		/// 
		/// The function:
		/// - creates the HWND using registered class,
		/// - sets application title,
		/// - returns the created window handle.
		/// 
		/// The returned HWND becomes the main Direct3D rendering surface.
		/// </summary>
		HWND WindowFactory(
			const RECT& wndRect,
			const I18N::I18NString& appTitle
		) const;
	};

	// =========================================================================
	// Main application message loop macro
	// =========================================================================
	//
	// This macro:
	//  - creates the main window class,
	//  - initializes the main window,
	//  - activates it,
	//  - initializes rendering,
	//  - runs the Win32 message loop,
	//  - shuts down cleanly.
	//
	// It is intentionally implemented as a macro to:
	//  - preserve call-site locality,
	//  - allow early return,
	//  - avoid boilerplate duplication across entry points.
	//
	// =========================================================================
#define MSG_LOOP \
	const WindowClassMain mainWindowClass{ WindowProc };\
	{\
		/* Abort startup if application is already closing */\
		if (Monad::Exceptions::InAnyCloseReason())\
			return EXIT_FAILURE;\
		\
		/* Create main window */\
		Monad::Globals::g_mainWindow = \
			mainWindowClass.WindowFactory( \
				GetMonitorRect(), \
				Monad::Globals::g_fullAppName \
			);\
	}\
	\
	/* Remove window decorations (fullscreen-style window) */\
	SetWindowLongPtr(Monad::Globals::g_mainWindow, GWL_STYLE, 0);\
	\
	/* Show and activate the window */\
	ShowWindow(Monad::Globals::g_mainWindow, SW_SHOWMAXIMIZED);\
	ActivateDirect3DWindow();\
	\
	/* Initialize rendering device */\
	g_singleton->OnCreateDevice();\
	\
	/* Standard Win32 message loop */\
	MSG msg{};\
	while (WM_QUIT != msg.message)\
	{\
		/* Process any messages in the queue.*/\
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))\
		{\
			TranslateMessage(&msg);\
			DispatchMessage(&msg);\
		}\
		/*else\
			OnBoth();*/\
	}\
	\
	/* Cleanup */\
	Monad::Globals::g_mainWindow = nullptr;\
	return static_cast<int32_t>(msg.wParam);
}