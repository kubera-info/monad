//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <cstdint>
// Monad
#include "Globals/__MonadGlobals.h"
#include "Wnd/Message.h"

enum WM_APPS : uint32_t
{
	WM_APP_ACTIVATE_WINDOW = WM_APP,
	WM_APP_CLICK,
	WM_APP_RELEASE,
	WM_APP_ON_THREAD_DONE
};

/// <summary>
/// A class containing a window class, a WindowProc, and a message loop for a DirectX 12 window.
/// </summary>
interface Win32ApplicationGeneric abstract
{
	static void SetTitle() noexcept;
	inline static bool IsIconic() noexcept { return ::IsIconic(Monad::Globals::g_mainWindow); }
	inline static void ActivateDirect3DWindow() noexcept { PostMessage(WM_APP_ACTIVATE_WINDOW); }
	inline static void PostMessage(
		uint32_t message,
		WPARAM wParam = 0,
		LPARAM lParam = 0
	) noexcept
	{
		/*if (nullptr != s_hwnd)*/ ::PostMessage(
			Monad::Globals::g_mainWindow,
			message,
			wParam,
			lParam
		);
	}
	static bool IsRendered() noexcept;
	static void OnBoth();
	static void OnTriple();
	static bool s_in_sizemove, s_inSuspend, s_minimized;
};

Monad::Wnd::ExpectedLResult ProcessWM_POINTERUPDATE(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_APP_ACTIVATE_WINDOW
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_ACTIVATEAPP
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessPBT_APMQUERYSUSPEND
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessPBT_APMRESUMESUSPEND
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_PAINT
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_NCHITTEST
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_SIZE
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_ENTERSIZEMOVE
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);

Monad::Wnd::ExpectedLResult ProcessWM_EXITSIZEMOVE
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
);