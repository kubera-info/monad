// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Window.h"
// Monad
#include "Globals/__MonadGlobals.h"
#include "resource.h"
#include "Exceptions/Exceptions.h"
#include "Files/FileManager.h"
#include "System/Exit.h"

namespace Monad
{
	using namespace Exceptions;
	using namespace Files;
	using namespace I18N;
	using namespace Input;

	namespace Wnd
	{
		// ---------------------------------------------------------------------
		// WindowClass
		// ---------------------------------------------------------------------

		WindowClass::WindowClass(
			const wchar_t* className,
			WNDPROC wndProc
		) :
			c_className{ className }
		{
			/// Registers a window class used for all application windows.
			/// This is performed once per application lifetime.

			const WNDCLASSEX wndClassEx
			{
				.cbSize = sizeof WNDCLASSEX,
				.lpfnWndProc = wndProc,
				.hInstance = g_hInstance,
				.hIcon = LoadIcon(Globals::g_sourceModuleHandle, MAKEINTRESOURCE(Globals::g_sourceIcon)),
				.hCursor = CURSOR_WAIT,
				.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
				.lpszClassName = className
			};

			if (FALSE == RegisterClassEx(&wndClassEx))
				THROW_EXC_GETLASTERROR(
					Exceptions::InvalidCall,
					L"Register Class Ex"
				);
		}

		WindowClass::~WindowClass()
		{
			/// Unregisters the window class during shutdown.
			/// Safe to call even if registration failed earlier.
			UnregisterClass(c_className.c_str(), g_hInstance);
		}

		// ---------------------------------------------------------------------
		// Window creation
		// ---------------------------------------------------------------------

		HWND WindowClass::WindowFactoryGeneric(
			const DWORD dwExStyle,
			const RECT& wndRect,
			LPVOID const lpParameter,
			const DWORD dwStyle,
			const I18NString& appTitle
		) const
		{
			/// Creates a window instance using the previously registered class.
			/// All parameters are passed directly to CreateWindowEx.

			if (const HWND hWnd = CreateWindowEx(
				dwExStyle,
				c_className.c_str(),
				appTitle.ToLPCWSTR(),
				dwStyle,
				wndRect.left,
				wndRect.top,
				wndRect.right - wndRect.left,
				wndRect.bottom - wndRect.top,
				0,
				nullptr,
				g_hInstance,
				lpParameter); nullptr == hWnd)
			{
				THROW_EXC_GETLASTERROR(
					Exceptions::NullValue,
					L"Create Window Ex"
				);
			}
			else
			{
				return hWnd;
			}
		}

		// ---------------------------------------------------------------------
		// Window helpers
		// ---------------------------------------------------------------------

		void SetWindowTitle(
			HWND pHWnd
		) noexcept
		{
			/// Updates window title using localized application name
			SetWindowText(
				pHWnd,
				Globals::g_fullAppName.ToLPCWSTR()
			);
		}
	}
}