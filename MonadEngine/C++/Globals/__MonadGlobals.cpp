// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadGlobals.h"

// Monad
#include "Kernel/__MonadKernel.h"

using namespace std;

namespace Monad::Globals
{
	using namespace Exceptions;
	using namespace Kernel;

	I18N::I18NString
		g_shortAppName{ L"noname"_en_US },
		g_fullAppName{ L"noname"_en_US },
		g_copyright{ L"Place your copyrignt/copyleft"_en_US };
	std::wstring
		g_whoAmI,
		g_adapterDesc,
		g_userFolder,
		g_userProfile;
	std::string g_sha256Salt;
	EXCEPTION_PRESENTATION
		g_exceptionReportingMethod = EXCEPTION_PRESENTATION::REPORT_EXE;
	Renderer::CB::PipeColor
		g_hrColors,
		g_borderColors;
	HWND g_mainWindow = nullptr;
	HINSTANCE g_sourceModuleHandle = nullptr;
	WORD g_sourceIcon = 0;
	HICON g_sourceIconHandle = nullptr;
}