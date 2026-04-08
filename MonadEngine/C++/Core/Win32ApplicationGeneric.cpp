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

bool Win32ApplicationGeneric::s_in_sizemove = false;
bool Win32ApplicationGeneric::s_inSuspend = false;
bool Win32ApplicationGeneric::s_minimized = false;

using Monad::Wnd::ExpectedLResult;

void Win32ApplicationGeneric::SetTitle() noexcept
{
	Monad::Wnd::SetWindowTitle(Monad::Globals::g_mainWindow);
}

bool Win32ApplicationGeneric::IsRendered() noexcept
{
	return g_dxSample
		&& !s_in_sizemove
		&& !s_minimized
		&& !s_inSuspend;
}

void Win32ApplicationGeneric::OnTriple()
{
	if (!IsRendered() || !g_dxSample->OnBoth())
	{
		PAINTSTRUCT ps;
		//HDC hdc;

		/*hdc =*/ static_cast<void>(BeginPaint(
			Monad::Globals::g_mainWindow,
			&ps)
			);
		EndPaint(
			Monad::Globals::g_mainWindow,
			&ps
		);
	}
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
	return Monad::Wnd::PROCESSING_OK;
}

Monad::Wnd::ExpectedLResult ProcessWM_APP_ACTIVATE_WINDOW
(
	HWND hWnd,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	Monad::Wnd::ActivateDirect3DWindow(hWnd);
	return Monad::Wnd::PROCESSING_OK;
}

Monad::Wnd::ExpectedLResult ProcessPBT_APMQUERYSUSPEND
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
	return Monad::Wnd::PROCESSING_TRUE;
}

Monad::Wnd::ExpectedLResult ProcessPBT_APMRESUMESUSPEND
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
	return Monad::Wnd::PROCESSING_TRUE;
}

Monad::Wnd::ExpectedLResult ProcessWM_PAINT
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	Win32ApplicationGeneric::OnTriple();
	return Monad::Wnd::PROCESSING_OK;
}

Monad::Wnd::ExpectedLResult ProcessWM_NCHITTEST
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
	Win32ApplicationGeneric::s_in_sizemove = true;
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
	Win32ApplicationGeneric::s_in_sizemove = false;
	return Monad::Wnd::PROCESSING_OK;
}

