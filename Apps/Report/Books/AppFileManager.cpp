// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppFileManager.h"
// Monad
#include "__App.h"
#include "AppSingleton.h"
#include "Globals/__MonadGlobals.h"
#include "Files/__MonadFile.h"
#include "Files/Handle.h"
#include "Fonts/FontLoader.h"
#include "Kernel/ComManager.h"
#include "Pages/Page.h"
#include "System/Parameter.h"
#include "Repository/Repo.h"
#include "Tools/Convert.h"

using namespace std;
namespace fs = filesystem;

namespace Monad::App
{
	using namespace Files;
	using namespace Kernel;
	using namespace Pages;
	using namespace Repositories;
	using namespace System;
	using namespace Tools;

	namespace
	{
		// Removes a file or directory without throwing exceptions
		void Remove(const fs::path& path) noexcept
		{
			std::error_code ec;
			fs::remove(path, ec);
		}

		/// <summary>
		/// Base asset directories used by the application.
		/// </summary>
		const Files::Path
			FOLDER_BOOT2{ DummyParameter{}, L"Boot2" },
			FOLDER_LOGO{ DummyParameter{}, L"Logo" },
			FOLDER_AUTO{ DummyParameter{}, L"Auto" },
			FOLDER_BOOT2_QUALITY{ FOLDER_BOOT2 / "Lo" },
			FOLDER_AUTO_QUALITY{ FOLDER_AUTO / "Lo" },
			FOLDER_STREAM{ DummyParameter{}, L"Stream" };
	}

	// ---------------------------------------------------------------------
	// TaskFileSearcher
	// ---------------------------------------------------------------------
	// Responsible for scanning application directories and loading
	// resources such as textures, sounds, fonts, shaders and profiles.
	// ---------------------------------------------------------------------

	TaskFileSearcher::TaskFileSearcher() :
		TaskFileSearcherGeneric{
		{
			{ L".monad-snd"s, LOADER_PCM },
			{ L".monad-msh"s, LOADER_MESH },
			{ L".monad-fnt"s, LOADER_FONT },
			{ L".monad-tex"s, LOADER_TEX },
			{ L".monad-vsi"s, LOADER_VS_IL },
			{ L".monad-psh"s, LOADER_PS }
		},
		g_singleton->m_cipher,
		g_singleton->m_threadPool
	}
	{
		// Initialize shader resource view count
		g_shaderResourceViewCount = SetShaderResourceViewsCount();

		// g_profilePath = GET_FOLDER_DOCUMENT / PROFILE_FILENAME;
	}

	// Executes file scanning depending on the current application state
	void TaskFileSearcher::Run()
	{
		switch (g_fileManagerState)
		{
		case LOAD_BOOT_DX:
		{
			g_fileManagerState = RUN_BOOT_DX;
			RunSearch({
				GetFontPath(Monad::Fonts::FONT_CENTURY_GOTHIC),
				FOLDER_LOGO / WILDCARD,
				FOLDER_BOOT2 / "*font*",
				FOLDER_BOOT2 / "*emoji*",
				FOLDER_BOOT2 / "*hr*",
				FOLDER_BOOT2 / "*border*",
				FOLDER_BOOT2 / "shadow.monad-psh",
				FOLDER_BOOT2_QUALITY / "*font*",
				FOLDER_BOOT2_QUALITY / "*emoji*",
				FOLDER_BOOT2_QUALITY / "*metal*",
				FOLDER_AUTO / "*button*",
				FOLDER_AUTO / "no.monad-tex",
				FOLDER_AUTO / "ok.monad-tex",
				FOLDER_AUTO / "iconic.monad-vsi",
				FOLDER_AUTO_QUALITY / "iconic.monad-psh",
				FOLDER_STREAM / "yes.monad-snd",
				FOLDER_STREAM / "no.monad-snd",
				}
				);
			break;
		}
		default:
			std::unreachable();
		}
	}

	// Handles transitions between file loading states
	void TaskFileSearcher::OnStateChange() noexcept
	{
		switch (g_fileManagerState)
		{
		case RUN_BOOT_DX: g_fileManagerState = STOP_BOOT_DX;
		}
	}

	uint32_t TaskFileSearcher::SetShaderResourceViewsCount()
	{
		constexpr uint32_t
			LOGO = 1,
			FONTS = 1,
			ICONIC_BUTTONS = 2,
			ALL_TEXTURES = LOGO + FONTS + ICONIC_BUTTONS;
		return ALL_TEXTURES;
	}
}