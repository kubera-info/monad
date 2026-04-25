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
#include "Win32Application.h"
// Platform
#include <Dbt.h>
// Monad
#include "AppSingleton.h"
#include "Core/Win32ApplicationGeneric.h"
#include "Exceptions/Exceptions.h"
#include "Globals/__MonadGlobals.h"
#include "Input/Cursor.h"
#include "Time/Timer.h"
#include "Wnd/WindowMain.h"
#include "Wnd/Message.h"

using namespace Monad::App;
using namespace Monad::Exceptions;
using namespace Monad::Files;
using namespace Monad::Globals;
using namespace Monad::I18N;
using namespace Monad::Input;
using namespace Monad::Time;
using namespace Monad::Wnd;

ExpectedLResult ProcessWM_CLOSE(
	HWND hWnd,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	if (!Monad::Exceptions::InAnyCloseReason()
		&& (STOP_BOOT_DX <= Monad::Files::g_fileManagerState
		&& FILE_MAN_STATE_SYSTEM > Monad::Files::g_fileManagerState
		|| FILE_MAN_STATE_READY == Monad::Files::g_fileManagerState)
		&& "exit"_pageNo != Monad::Pages::g_currentPage)
	{
		Monad::Pages::SelectPage("exit"_pageNo);
		return PROCESSING_ERROR;
	}
	else
	{
		RegisterWM_CLOSE();
		g_singleton->OnDestroyDevice();
		DestroyWindow(hWnd);
		return PROCESSING_OK;
	}
}

ExpectedLResult ProcessWM_APP_RELEASE(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	IsMouseDownStub(false);
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_POINTERUP(
	HWND,
	UINT,
	WPARAM,
	LPARAM lParam,
	LPVOID&
)
{
	SetCursorPos(lParam);
	IsMouseDownStub(false);
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_APP_CLICK(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	IsMouseDownStub(true);
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_POINTERDOWN(
	HWND,
	UINT,
	WPARAM,
	LPARAM lParam,
	LPVOID&
)
{
	SetCursorPos(lParam);
	IsMouseDownStub(true);
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_DISPLAYCHANGE
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	// Monitor configuration changed
	g_singleton->OnReorganize();
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_DEVICECHANGE
(
	HWND,
	UINT,
	WPARAM wParam,
	LPARAM,
	LPVOID&
)
{
	if (std::set<WPARAM>{
		DBT_DEVICEARRIVAL,
			DBT_DEVICEREMOVECOMPLETE,
			DBT_DEVNODES_CHANGED
	}.contains(wParam))
		g_singleton->OnReorganize();
	return PROCESSING_OK;
}

ExpectedLResult ProcessWM_SETCURSOR
(
	HWND,
	UINT,
	WPARAM,
	LPARAM,
	LPVOID&
)
{
	g_singleton->m_cursorControl.SetCurrentCursor();
	return PROCESSING_TRUE;
}

Monad::Wnd::ExpectedLResult ProcessWM_ACTIVATEAPP
(
	HWND,
	UINT,
	WPARAM wParam,
	LPARAM,
	LPVOID&
)
{
	// Application focus changed
	g_active = TRUE == wParam;
	if (g_active)
		g_singleton->m_cursorControl.SetCurrentCursor();
	return PROCESSING_OK;
}

MapForMessages g_forMainMessage{
	MONAD_MSG(
		WM_CLOSE
	),
	MONAD_MSG(
		WM_POINTERUPDATE
	),
	MONAD_MSG(
		WM_APP_RELEASE
	),
	MONAD_MSG(
		WM_APP_CLICK
	),
	MONAD_MSG(
		WM_POINTERDOWN
	),
	MONAD_MSG(
		WM_DISPLAYCHANGE
	),
	MONAD_MSG(
		WM_DEVICECHANGE
	),
	MONAD_MSG(
		WM_SETCURSOR
	),
	MONAD_MSG(
		WM_APP_ACTIVATE_WINDOW
	),
	MONAD_MSG(
		WM_ACTIVATEAPP
	),
	MONAD_MSG(
		PBT_APMQUERYSUSPEND
	),
	MONAD_MSG(
		PBT_APMRESUMESUSPEND
	),
	MONAD_MSG(
		WM_PAINT
	),
	MONAD_MSG(
		WM_NCHITTEST
	),
	MONAD_MSG(
		WM_SIZE
	),
	MONAD_MSG(
		WM_ENTERSIZEMOVE
	),
	MONAD_MSG(
		WM_EXITSIZEMOVE
	)
};

/// <summary>
/// Main application loop entry point.
/// </summary>
int32_t Win32Application::Run()
{
	MSG_LOOP;
}

/// <summary>
/// Main Win32 message handler.
/// Handles input, window events, power events and application lifecycle.
/// </summary>
LRESULT CALLBACK Win32Application::WindowProc(
	HWND hWnd,
	uint32_t uMsg,
	WPARAM wParam,
	LPARAM lParam
) noexcept
{
	try
	{
		LPVOID dummyContext = nullptr;
		// Handle shared / common messages first
		if (const auto lres = OnCommonMessages2(
			{ g_forMainMessage, g_forMessage },
			hWnd,
			uMsg,
			wParam,
			lParam,
			dummyContext
		); lres.has_value())
			return *lres;
	}
	MONAD_CATCH;

	return DefWindowProc(
		hWnd,
		uMsg,
		wParam,
		lParam
	);
}