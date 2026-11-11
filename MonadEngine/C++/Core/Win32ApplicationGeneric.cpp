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

// Me
#include "stdafx.h"
#include "Win32ApplicationGeneric.h"

// Monad
#include "DXSample.h"
#include "Wnd/WindowMain.h"
#include "Input/Cursor.h"
#include "Time/__MonadTime.h"
#include "Time/Timer.h"

bool Win32ApplicationGeneric::s_inSizeMove = false;
bool Win32ApplicationGeneric::s_inSuspend = false;
bool Win32ApplicationGeneric::s_minimized = false;

using namespace Monad::Wnd;

void Win32ApplicationGeneric::SetTitle() noexcept
{
	SetWindowTitle(Monad::Globals::g_mainWindow);
}

bool Win32ApplicationGeneric::IsRendered() noexcept
{
	return g_dxSample
		&& !s_inSizeMove
		&& !s_minimized
		&& !s_inSuspend;
}

void Win32ApplicationGeneric::OnBoth()
{
	if (IsRendered())
		g_dxSample->OnBoth();
}

ExpectedLResult ProcessWM_POINTERUPDATE(
	HWND,
	UINT,
	WPARAM,
	LPARAM lParam,
	LPVOID&
)
{
	// Pointer moved (mouse, pen, touch)
	Monad::Input::SetCursorPos(lParam);
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_APP_ACTIVATE_WINDOW
(
	HWND hWnd,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	ActivateDirect3DWindow(hWnd);
	return PROCESSING_OK;
}

ExpectedLResult ProcessPBT_APMQUERYSUSPEND
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	if (!Win32ApplicationGeneric::s_inSuspend)
		Monad::Input::g_active = false;
	Win32ApplicationGeneric::s_inSuspend = true;
	return PROCESSING_TRUE;
}

ExpectedLResult ProcessPBT_APMRESUMESUSPEND
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	// System resumed from suspend
	if (!Win32ApplicationGeneric::s_minimized)
	{
		if (Win32ApplicationGeneric::s_inSuspend)
			Monad::Input::g_active = true;
		Win32ApplicationGeneric::s_inSuspend = false;
	}
	return PROCESSING_TRUE;
}

ExpectedLResult ProcessWM_PAINT
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	Win32ApplicationGeneric::OnBoth();
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_NCHITTEST
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	// Prevent window menu activation in fullscreen mode
	static_assert(HTCLIENT == TRUE);
	return Monad::Wnd::PROCESSING_TRUE;
}

Monad::Wnd::ExpectedLResult ProcessWM_SIZE
(
	HWND,
	UINT,
	WPARAM wParam,
	LPARAM,
	LPVOID&
)
{
	if (wParam == SIZE_MINIMIZED)
	{
		if (!Win32ApplicationGeneric::s_minimized)
		{
			Win32ApplicationGeneric::s_minimized = true;
			if (!Win32ApplicationGeneric::s_inSuspend)
				Monad::Input::g_active = false;
			Win32ApplicationGeneric::s_inSuspend = true;
		}
	}
	else if (Win32ApplicationGeneric::s_minimized)
	{
		Win32ApplicationGeneric::s_minimized = false;
		if (Win32ApplicationGeneric::s_inSuspend)
			Monad::Input::g_active = true;
		Win32ApplicationGeneric::s_inSuspend = false;
	}
	return Monad::Wnd::PROCESSING_OK;
}

Monad::Wnd::ExpectedLResult ProcessWM_ENTERSIZEMOVE
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	Win32ApplicationGeneric::s_inSizeMove = true;
	return Monad::Wnd::PROCESSING_OK;
}

Monad::Wnd::ExpectedLResult ProcessWM_EXITSIZEMOVE
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	Win32ApplicationGeneric::s_inSizeMove = false;
	return Monad::Wnd::PROCESSING_OK;
}