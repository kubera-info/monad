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
#include "AppXMLProfile.h"
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

		// Removes legacy profile formats and related files
		void RemoveModernPack(const fs::path& path)
		{
			Remove(path / "profile.monad-dbf");
			Remove(path / "profile.monad-dbf~");
			Remove(path);
		}

		/// <summary>
/// Base asset directories used by the application.
/// </summary>
		const Files::Path
			FOLDER_BOOT{ DummyParameter{}, "Boot" },
			FOLDER_BOOT2{ DummyParameter{}, "Boot2" },
			FOLDER_EMOJI{ DummyParameter{}, "Emoji" },
			FOLDER_STREAM{ DummyParameter{}, "Stream" },
			FOLDER_LOGO{ DummyParameter{}, "Logo" },
			FOLDER_ART{ DummyParameter{}, "Art" },
			FOLDER_AUTO{ DummyParameter{}, "Auto" },
			FOLDER_BOOT2_WALLPAPER{
				FOLDER_BOOT2 /
					VectorRandomString{ "0", "1", "2" }.GetRandomString() };

		/// <summary>
		/// Runtime-dependent asset directories
		/// (quality level, language, etc.).
		/// </summary>
		Files::Path
			m_folderBoot2Quality,
			m_folderStreamI18N,
			m_folderAutoQuality;
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
			{ L".monad-stm"s, LOADER_MP3 },
			{ L".monad-msh"s, LOADER_MESH },
			{ L".monad-dbf"s, [](TaskFileHolderGeneric& file)
				{
					LoadXMLProfile(file.GetComIStreamTemp().Get());
				}
			},
			{ L".monad-fnt"s, LOADER_FONT },
			{ L".monad-tex"s, LOADER_TEX },
			{ EXT_ART, [](TaskFileHolderGeneric& file)
				{
					assert(file.HasArchive());
					g_repository->m_memArts.Insert(
						file.GetFilename(),
						move(file.m_archiveBuffer)
					);
				}
			},
			{ L".monad-vsi"s, LOADER_VS_IL },
			{ L".monad-psh"s, LOADER_PS },
			{ L".monad-lob"s, LOADER_DUMMY }
		},
		g_singleton->m_cipher,
		g_singleton->m_threadPool
	}
	{
		// Initialize shader resource view count
		g_shaderResourceViewCount = SetShaderResourceViewsCount();

		// Cleanup of legacy profile directories
		{
			const auto ver1_0 = GetFolderAppDocumentsSettings(
				Globals::g_userFolder,
				"1.0"
			);

			const auto ver4_2_0 = GetFolderAppDocumentsSettings(
				Globals::g_userFolder,
				"4.2.0"
			);

			const auto ver4_6_5 = GetFolderAppDocumentsSettings(
				Globals::g_userFolder,
				"4.6.5"
			);

			const auto ver4_8_2 = GetFolderAppDocumentsSettings(
				Globals::g_userFolder,
				"4.8.2"
			);
			const auto ver4_8_3 = GetFolderAppDocumentsSettings(
				Globals::g_userFolder,
				"4.8.3"
			);

			Remove(ver1_0 / "profile.monad-db");
			Remove(ver1_0);
			RemoveModernPack(ver4_2_0);
			RemoveModernPack(ver4_6_5);
			RemoveModernPack(ver4_8_2);
			RemoveModernPack(ver4_8_3);
		}

		g_profilePath = GET_FOLDER_DOCUMENT / PROFILE_FILENAME;
	}
	// Sets runtime-dependent resource paths
	void TaskFileSearcher::SetPaths()
	{
		m_folderBoot2Quality = FOLDER_BOOT2 / g_applied.GetStringRendererQuality();
		m_folderStreamI18N = FOLDER_STREAM / I18N::GetLocaleName();
		m_folderAutoQuality = FOLDER_AUTO / g_applied.GetStringRendererQuality();
	}
	// Executes file scanning depending on the current application state
	void TaskFileSearcher::Run()
	{
		using InitializerListPaths = std::initializer_list<fs::path>;
		VectorPath searchPaths;

		switch (g_fileManagerState)
		{
		case FILE_MAN_STATE_LOAD_BOOT:
		{
			g_fileManagerState = RUN_BOOT;
			searchPaths.emplace_back(
				FOLDER_BOOT / WILDCARD
			);
			break;
		}
		case LOAD_BOOT_DX:
		{
			g_fileManagerState = RUN_BOOT_DX;
			searchPaths.assign_range<InitializerListPaths>({
				FOLDER_BOOT2 / WILDCARD,
				FOLDER_BOOT2_WALLPAPER / WILDCARD,
				m_folderBoot2Quality / WILDCARD,
				GetFontPath(Fonts::FONT_CENTURY_GOTHIC)
				});
			break;
		}
		case LOAD_ALL:
		{
			g_fileManagerState = RUN_ALL;
			searchPaths.assign_range<InitializerListPaths>({
				FOLDER_LOGO / WILDCARD,
				FOLDER_AUTO / WILDCARD,
				m_folderAutoQuality / WILDCARD,
				GetFontPath(Fonts::FONT_TIMES_NEW_ROMAN)
				});

			if (static SingleCall bOnceToLoad; !bOnceToLoad.test_and_set())
			{
				searchPaths.append_range<InitializerListPaths>({
					m_folderStreamI18N / WILDCARD,
					FOLDER_STREAM / WILDCARD,
					FOLDER_ART / WILDCARD
					});
			}
			else if (Pages::g_langChanged2)
			{
				searchPaths.emplace_back(
					m_folderStreamI18N / WILDCARD
				);
				Pages::g_langChanged2 = false;
			}
			break;
		}
		case LOAD_PROFILE:
		{
			g_fileManagerState = RUN_PROFILE;
			searchPaths.emplace_back(
				GET_FOLDER_DOCUMENT / PROFILE_FILENAME
			);
			break;
		}
		case LOAD_I18N_DESC:
		{
			g_fileManagerState = RUN_I18N_DESC;
			searchPaths.emplace_back(
				m_folderStreamI18N / WILDCARD
			);
			break;
		}
		default:
			std::unreachable();
		}

		RunSearch(searchPaths);
	}

	// Handles transitions between file loading states
	void TaskFileSearcher::OnStateChange() noexcept
	{
		switch (g_fileManagerState)
		{
		case RUN_BOOT:        g_fileManagerState = STOP_BOOT; break;
		case RUN_BOOT_DX:     g_fileManagerState = STOP_BOOT_DX; break;
		case RUN_ALL:         g_fileManagerState = STOP_ALL; break;
		case RUN_I18N_DESC:   g_fileManagerState = Files::FILE_MAN_STATE_READY; break;
		default:              g_fileManagerState = STOP_PROFILE;
		}
	}

	uint32_t TaskFileSearcher::SetShaderResourceViewsCount()
	{
		return __super::SetShaderResourceViewsCount();
	}
}