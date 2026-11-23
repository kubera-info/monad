// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "StickyKeys.h"

namespace Monad::Input
{
	namespace
	{
		// Save the current sticky/toggle/filter key settings so program can restore them later.
		STICKYKEYS g_sk{ sizeof STICKYKEYS, 0 };
		TOGGLEKEYS g_tk{ sizeof TOGGLEKEYS, 0 };
		FILTERKEYS g_fk{ sizeof FILTERKEYS, 0 };
	}

	StickyKeys::StickyKeys() noexcept
	{
		if (!SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof STICKYKEYS, &g_sk, 0))
			memset(&g_sk, 0, sizeof g_sk);
		if (!SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof TOGGLEKEYS, &g_tk, 0))
			memset(&g_tk, 0, sizeof g_tk);
		if (!SystemParametersInfo(SPI_GETFILTERKEYS, sizeof FILTERKEYS, &g_fk, 0))
			memset(&g_fk, 0, sizeof g_fk);
		AllowShortcutKeys(false);
	}

	StickyKeys::~StickyKeys()
	{
		AllowShortcutKeys(true);
	}

	void StickyKeys::AllowShortcutKeys(
		bool allowKeys
	) noexcept
	{
		if (allowKeys)
		{
			SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &g_sk, 0);
			SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_tk, 0);
			SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &g_fk, 0);
		}
		else
		{
			STICKYKEYS skOff = g_sk;
			if ((skOff.dwFlags & SKF_STICKYKEYSON) == 0)
			{
				// Disable the hotkey and the confirmation.
				skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
				skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

				SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
			}

			TOGGLEKEYS tkOff = g_tk;
			if ((tkOff.dwFlags & TKF_TOGGLEKEYSON) == 0)
			{
				// Disable the hotkey and the confirmation.
				tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
				tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;

				SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
			}

			FILTERKEYS fkOff = g_fk;
			if ((fkOff.dwFlags & FKF_FILTERKEYSON) == 0)
			{
				// Disable the hotkey and the confirmation.
				fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
				fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;

				SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
			}
		}
	}
}
