// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
// Monad
#include "resource.h"
#include "Globals/__MonadGlobals.h"
#include "AppSingleton.h"
#include "Win32Application.h"
#include "I18N/I18N.h"
#include "Exceptions/Exceptions.h"
#include "GUI/Dialog.h"
#include "Kernel/Main.h"
#include "System/ResourceLoader.h"

using namespace Monad;
using namespace Monad::I18N;

/// <summary>
/// Application entry point used by the engine bootstrap.
/// Initializes global application metadata and starts the main engine loop.
/// </summary>
/// <param name="hInstance">Handle to the application instance.</param>
/// <returns>Application exit code.</returns>
int32_t WINAPI MiniMain(
	_In_ HINSTANCE hInstance
)
{
	MONAD_RAW_INIT;

	// Application display names (localized)
	Globals::g_shortAppName =
	{
		L"#emoji_u1f3a8#Zaczarowana Kredka"_pl,
		L"#emoji_u1f3a8#Волшебные Краски"_ru,
		L"#emoji_u1f3a8#The Enchanted Crayon"_us
	};

	Globals::g_fullAppName =
	{
		L"Kolorowanka „Zaczarowana Kredka”"_pl,
		L"Раскраска «Волшебные Краски»"_ru,
		L"The Enchanted Crayon Colouring Book"_gb,
		L"The Enchanted Crayon Coloring Book"_us
	};

	// Application identifiers
	Globals::g_whoAmI = L"ZC";
	Globals::g_userFolder = L"Enchanted Crayon";
	Globals::g_userProfile = L"SOFTWARE\\" L"Wlodzimierz O. Kubera\\Enchanted Crayon\\" APP_VER;
	Globals::g_sha256Salt = "0AFFC219D06D4F89";
	Globals::g_sourceIcon = IDI_MAIN_ICON;

	// Copyright text
	Globals::g_copyright =
	{
		L"Copyright © 2016-2026 by Włodzimierz O. Kubera"_pl,
		L"Copyright © 2016-2026 by Wlodzimierz O. Kubera"_us
	};

	// Default UI colors (HDR space)
	Globals::g_hrColors = {
		Renderer::HDR::GetSRGB({ 128.0f, 16.0f, 16.0f }),
		Renderer::FLOAT_4_ONES,
		{ 0.125f, 0.125f, 0.0025f, 1.0f }
	};

	Globals::g_borderColors = {
		Renderer::HDR::GetSRGB({ 12.0f, 4.0f, 80.0f }),
		{ 0.8125f, 0.8125f, 0.8125f, 1.0f },
		{ 0.05f, 0.05f, 0.00125f, 1.0f }
	};

	// Supported texture extensions
	Files::g_texturesExts = {
		Files::EXT_ART,
		Files::EXT_FONT,
		Files::EXT_TEX
	};

	// Start the application framework
	return Kernel::MonadWinMain<
		App::PersistentSingleton,
		Win32Application>(
			Wnd::GetMonitorRect()
		);
}

/// <summary>
/// Standard Windows entry point.
/// Wraps MiniMain and provides structured exception handling when enabled.
/// </summary>
int32_t WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ wchar_t*,
	_In_ int32_t)
{
#if MONAD_HARDWARE_EXCEPTIONS == 1
	__try
	{
#endif
		return MiniMain(hInstance);
#if MONAD_HARDWARE_EXCEPTIONS == 1
	}
	__except (Exceptions::filter(GetExceptionCode(), GetExceptionInformation()))
	{
		return EXIT_FAILURE;
	}
#endif
}