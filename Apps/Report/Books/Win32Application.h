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
#include <shobjidl.h>

// Monad
#include "Core/Win32ApplicationGeneric.h"
#include "Wnd/Message.h"

extern std::atomic_bool g_keyPressed;

/// <summary>
/// Main Win32 application entry class.
/// 
/// This class provides:
/// - a Win32 window class definition,
/// - a message loop,
/// - and a static WindowProc handler
/// used by the DirectX 12 rendering framework.
/// </summary>
struct Win32Application : Win32ApplicationGeneric
{
	/// <summary>
	/// Starts the main application loop.
	/// </summary>
	/// <returns>Application exit code.</returns>
	static int32_t Run();

private:
	/// <summary>
	/// Main window procedure handling Win32 messages.
	/// 
	/// Responsible for:
	/// - input processing,
	/// - window state changes,
	/// - system and power notifications,
	/// - forwarding events to the engine core.
	/// </summary>
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		uint32_t message,
		WPARAM wParam,
		LPARAM lParam
	) noexcept;
};

extern Monad::Wnd::MapForMessages g_forMainMessage;