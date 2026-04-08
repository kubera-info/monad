// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
// Monad
#include "AppMain.h"
#include "Globals/__MonadGlobals.h"
#include "AppSingleton.h"
#include "Win32Application.h"
#include "I18N/I18N.h"
#include "Exceptions/Exceptions.h"
#include "GUI/Dialog.h"
#include "Files/Op.h"
#include "Files/FileManager.h"
#include "Kernel/Main.h"
#include "BugXML.h"
#include "System/Parameter.h"
#include "AppParameter.h"

using namespace Monad;
using namespace Monad::I18N;

inline void LoadParametersFromFile()
{
	Kernel::VectorBytes fileBuffer;
	System::SmartHandle event(OpenEvent(EVENT_MODIFY_STATE, FALSE, Exceptions::g_childEvent));
	const Files::SmartHandleFile fileReader(
		(HANDLE)(UINT_PTR)wcstoull(System::GetParameter(System::FILENAME_REPORT).c_str(), NULL, 10)
	);
	fileBuffer.resize(fileReader.FileSize());
	fileReader.Read(fileBuffer);
	SetEvent(event);
	// Open the event
	g_parameters = std::make_unique<XMLManagerErrorDesc>(Monad::Kernel::SpanConstBytes(fileBuffer).GetComIStreamTemp().Get());
}
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

	Globals::g_exceptionReportingMethod = Exceptions::EXCEPTION_PRESENTATION::MESSAGE_BOX;

	// Application identifiers
	Globals::g_whoAmI = L"ER";
	
	// Copyright text
	Globals::g_copyright =
	{
		L"Copyright © 2016-2026 by Włodzimierz O. Kubera"_pl,
		L"Copyright © 2016-2026 by Wlodzimierz O. Kubera"_us
	};

	// Default UI colors (HDR space)
	Globals::g_hrColors = {
		Renderer::HDR::GetSRGB({ 16.0f, 128.0f, 16.0f }),
		Renderer::FLOAT_4_ONES,
		{ 0.125f, 0.75f, 0.0025f, 1.0f }
	};

	Globals::g_borderColors = {
		Renderer::HDR::GetSRGB({ 132.0f, 64.0f, 20.0f }),
		{ 0.8125f, 0.8125f, 0.8125f, 1.0f },
		{ 0.5f, 0.25f, 0.00125f, 1.0f }
	};

	// Supported texture extensions
	Files::g_texturesExts = {
		Files::EXT_FONT,
		Files::EXT_TEX
	};

	Monad::Globals::g_exceptionReportingMethod = Monad::Exceptions::EXCEPTION_PRESENTATION::MESSAGE_BOX;
	std::set_terminate(Monad::Exceptions::ReportAndTerminate);
#if defined _DEBUG
	if (System::GetParameter(System::FILENAME_REPORT).empty())
	{
		// Mocked call
		const std::string xmlForDebug{ "<monad I18N=\"en-GB\" Thread=\"singleton_pool\" Class=\"struct Monad::Exceptions::IOError\" Exception=\"Failure to access the data disk\" HRESULT=\"0x80070003\" Stage=\"FS Cannot Find Files: *.monad-???\" Page=\"boot\" Type=\"Report\" GPU=\"NVIDIA GeForce RTX 5070 Ti Laptop GPU\" Version=\"4.8.3 X64\" Title=\"The Enchanted Crayon Colouring Book\" Salt=\"0AFFC219D06D4F89\" ShortName=\"#emoji_u1f3a8#The Enchanted Crayon\" Path=\"C:\\Users\\wlodz\\source\\repos\\monad_solution\\Apps\\EnchantedCrayon\\Debug\\Book.exe\" WhoAmI=\"ZC\" NativeOrWine=\"Native Windows\" Arch=\"X64\" Ico=\"101\"/>" };
		g_parameters = std::make_unique<XMLManagerErrorDesc>(Kernel::SpanConstBytes(xmlForDebug).GetComIStreamTemp().Get());
	}
	else
		LoadParametersFromFile();
#else
	LoadParametersFromFile();
#endif
	Globals::g_sha256Salt =
		Tools::UnicodeToUTF8(g_parameters->GetParameter(System::PAR_NAME_SALT));
	Globals::g_shortAppName =
		g_parameters->GetParameter(System::PAR_NAME_SHORT_NAME);
	Globals::g_fullAppName =
		g_parameters->GetParameter(System::PAR_NAME_APP_TITLE);

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
		const auto ret = MiniMain(hInstance);
		// Since the dialog has been closed, return FALSE so that we exit the
		//  application, rather than start the application's message pump.
		//std::error_code ec;
		//std::filesystem::remove(System::FILENAME_REPORT, ec);
		return ret;
#if MONAD_HARDWARE_EXCEPTIONS == 1
	}
	__except (Exceptions::filter(GetExceptionCode(), GetExceptionInformation()))
	{
		return EXIT_FAILURE;
	}
#endif
}