// ############################################################################
// ### Copyright © 2016-2024 by Włodzimierz O. Kubera. All rights reserved. ###
// ############################################################################

//-----------------------------------------------------------------------------
// File: D3DSaver.cpp
//
// Desc: Framework for screensavers that use Direct3D 8.0.
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Saver.h"

namespace Monad::Screensaver
{
	HWND GetWindow(
		LPCWSTR commandLine
	)
	{
		commandLine++;
		while (*commandLine && !isdigit(*commandLine))
			commandLine++;
		static_assert(sizeof HWND == sizeof(unsigned long long));
		if (0 != *commandLine)
			return reinterpret_cast<HWND> (wcstoull(commandLine, nullptr, 10));
		else
			return nullptr;
	}

	ScreensaverParameters::ScreensaverParameters(
		const SCREENSAVER_MODE saverMode,
		const HWND wndParent
	) noexcept :
		c_screensaverMode(saverMode),
		c_hWndParent(wndParent)
	{
	}

	bool ScreensaverParameters::IsFullScreen() const throw()
	{
		return SCREENSAVER_MODE::FULL == c_screensaverMode || SCREENSAVER_MODE::TEST == c_screensaverMode;
	}

//-----------------------------------------------------------------------------
// Name: GetScreenSaverParameters()
// Desc: Interpret command-line parameters passed to this app.
//-----------------------------------------------------------------------------
ScreensaverParameters GetScreensaverParameters() noexcept
{
	LPCWSTR commandLine = GetCommandLine();

		// Skip the first part of the command line, which is the full path 
		// to the exe.  If it contains spaces, it will be contained in quotes.
		if (*commandLine == L'\"')
		{
			commandLine++;
			while (*commandLine != L'\0' && *commandLine != L'\"')
				commandLine++;
			if (*commandLine == L'\"')
				commandLine++;
		}
		else
		{
			while (*commandLine != L'\0' && *commandLine != L' ')
				commandLine++;
			if (*commandLine == L' ')
				commandLine++;
		}

		// Skip along to the first option delimiter "/" or "-"
		while (*commandLine != L'\0' && *commandLine != L'/' && *commandLine != L'-')
			commandLine++;

		// If there wasn't one, then must be config mode
		if (*commandLine == L'\0')
			return SCREENSAVER_MODE::CONFIG;

		// Otherwise see what the option was
		switch (toupper(*(++commandLine)))
		{
		case 'C':
			return
			{
				SCREENSAVER_MODE::CONFIG,
				GetWindow(commandLine)
			};

		case 'T':
			return SCREENSAVER_MODE::TEST;

		case 'P':
			// Preview-mode, so option is followed by the parent HWND in decimal
			return
			{
				SCREENSAVER_MODE::PREVIEW,
				GetWindow(commandLine)
			};

		case 'A':
			// Password change mode, so option is followed by parent HWND in decimal
			return SCREENSAVER_MODE::ERROR_IN_EXEC;

		default:
			// All other options => run the screensaver (typically this is "/s")
			return SCREENSAVER_MODE::FULL;
		}
	}
}