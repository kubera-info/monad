// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>
// Monad
#include "Exceptions/__MonadException.h"
#include "Renderer/__MonadRenderer.h"

namespace Monad::Globals
{
	/// <summary>
	/// Vector of regular expressions used to resolve a simplified locale.
	/// Resolution order: detected current locale first, then en-US as fallback.
	/// </summary>
	extern I18N::VectorSimplifiedLocaleNames g_simplifiedLocaleNames;
	/// <summary>
	/// Application name,
	/// Copyright notice, author, ©, year, etc.
	/// </summary>
	extern I18N::I18NString
		g_shortAppName,
		g_fullAppName,
		g_copyright;
	/// <summary>
	/// The id of the app's type (a short string),
	/// Filesystem path to the user’s Documents directory,
	/// Path to the registry key.
	/// </summary>
	extern std::wstring
		g_whoAmI,
		g_adapterDesc,
		g_userFolder,
		g_userProfile;
	extern std::string g_sha256Salt;
	/// <summary>
	/// Specifies how exceptions are processed: via MessageBox, reporting system, or console output.
	/// </summary>
	extern Exceptions::EXCEPTION_PRESENTATION g_exceptionReportingMethod;
	/// <summary>
	/// Defines the visual style and appearance of dialog windows.
	/// </summary>
	extern Renderer::CB::PipeColor
		g_hrColors,
		g_borderColors;
	/// <summary>
	/// Primary 3D rendering window.
	/// </summary>
	extern HWND g_mainWindow;
	extern HINSTANCE g_sourceModuleHandle;
	extern WORD g_sourceIcon;
	extern HICON g_sourceIconHandle;
}
