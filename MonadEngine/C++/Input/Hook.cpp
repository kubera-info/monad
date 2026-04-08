// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Hook.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Input
{
	Hook::Hook(
		HHOOK& hookHandle,
		int32_t hookID,
		HOOKPROC procedure
	) :
		m_hookHandle{ hookHandle }
	{
		if ((m_hookHandle = SetWindowsHookEx(
			hookID,
			procedure,
			g_hInstance,
			0
		)) == NULL)
			THROW_EXC_GETLASTERROR(Exceptions::NotInitialized, L"Set Windows Hook Ex")
	}
	Hook::~Hook()
	{
		UnhookWindowsHookEx(m_hookHandle);
		m_hookHandle = nullptr;
	}
}