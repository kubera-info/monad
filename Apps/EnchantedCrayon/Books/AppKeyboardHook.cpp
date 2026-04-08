// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppKeyboardHook.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad
{
	using namespace Input;

	namespace App
	{
		namespace
		{
			HHOOK g_hKeyboardHook = nullptr;

			LRESULT CALLBACK LowLevelKeyboardProc(
				int32_t nCode,
				WPARAM wParam,
				LPARAM lParam
			) noexcept
			{
				if (HC_ACTION == nCode)
					if (const KBDLLHOOKSTRUCT* const p = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam); p->vkCode == VK_LWIN || p->vkCode == VK_RWIN)
						if (WM_KEYDOWN == wParam || WM_KEYUP == wParam)
							return 1;
				return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
			}
		}

		KeyboardHookNoWinKey::KeyboardHookNoWinKey() :
			Hook(
				g_hKeyboardHook,
				WH_KEYBOARD_LL,
				LowLevelKeyboardProc
			)
		{}
	}
}