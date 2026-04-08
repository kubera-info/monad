// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppSingleton.h"
// Monad
#include "Globals/__MonadGlobals.h"
#include "Core/DXSample.h"
#include "Exceptions/Exceptions.h"
#include "GUI/Frame.h"
#include "Win32Application.h"
#include "Files/Registry.h"
#include "Fonts/__MonadFont.h"
#include "Input/Cursor.h"
#include "Renderer/States.h"
#include "System/Exit.h"
#include "Threads/__MonadThread.h"
#include "System/Parameter.h"
#include "Tools/Convert.h"
#include "System/ResourceLoader.h"

using namespace std;
namespace fs = filesystem;

namespace Monad
{
	using namespace Files;
	using namespace Fonts;
	using namespace I18N;
	using namespace Exceptions;
	using namespace Input;
	using namespace Kernel;
	using namespace Renderer;
	using namespace Renderer::InputLayout;
	using namespace Threads;
	using namespace Wnd;

	namespace App
	{
		PersistentSingleton::Config::ConfigRecordGlobal g_applied;
		namespace
		{
			inline RENDERER_QUALITIES GetValidatedQuality(
				const uint32_t qualityFromReg
			) noexcept
			{
				return RENDERER_QUALITY_HI >= qualityFromReg ?
					static_cast<RENDERER_QUALITIES>(qualityFromReg) :
					RENDERER_QUALITY_MED;
			}
		}

#pragma region PersistentSingleton
		PersistentSingleton::PersistentSingleton() :
			c_me{
			this,
			g_singleton
		},
			m_audioManager{ g_applied.m_audio, { Audio::STREAM, Audio::JINGLE, Audio::DESCRIBE } },
			m_threadPool{
				THREAD_NAME_SINGLETON_POOL,
				GetOptimalMapSize({
					{16U, 8U},
					{12U, 6U},
					{8U, 5U},
					{6U, 4U} },
					thread::hardware_concurrency(),
					2U)
			},
			m_satellitesManager{ m_2D }
		{
			m_cursorControl.SetNewCurrentCursor(CURSOR_WAIT);
			InsertTaskSearcher();
		}

		void PersistentSingleton::OnCreateDevice()
		{
			m_fileSearcher.SetPaths();
			const auto r = GetMonitorRect();
			MoveWindow(Monad::Globals::g_mainWindow, r.left, r.top, r.right - r.left, r.bottom - r.top, FALSE);
			DXSample::UpdateForSizeChange(r.right - r.left, r.bottom - r.top);
			m_d3dSingleton = make_unique<CombineD3DSingleton>();
		}

		void PersistentSingleton::CreateFull()
		{
			m_d3dSingleton->CreateFull();
		}

		void PersistentSingleton::OnDestroyDevice() noexcept
		{
			m_fileSearcher.WaitForStateCompletion();
			Pages::g_hourGlassBlockingRefs.Wait();
			m_repository.OnClearD3DAssets();

			Pages::SetMyCursor();
			m_cursorControl.SetNewCurrentCursor(CURSOR_WAIT);
			m_d3dSingleton.reset();

			g_fileManagerState = STOP_BOOT;
			Pages::g_processingPages = false;
		}

		void PersistentSingleton::InsertTask(
			Threads::ThreadPool::ITask* const task
		)
		{
			m_threadPool.InsertTask(task);
		}

		void PersistentSingleton::InsertTaskSearcher()
		{
			InsertTask(&m_fileSearcher);
		}

		void PersistentSingleton::InsertTaskSearcher2(
			const FILE_MAN_STATES newState
		)
		{
			g_fileManagerState = newState;
			InsertTaskSearcher();
		}

		void PersistentSingleton::ArrangeSatellites()
		{
			//const lock_guard lg(System::g_safeShutdown);
			m_satellitesManager.SetSatelliteWindows();
		}

		bool PersistentSingleton::IsFull() const noexcept
		{
			return m_d3dSingleton->m_world && m_d3dSingleton->m_world->m_full;
		}

		void PersistentSingleton::SetPalette() noexcept
		{
			m_repository.m_currentArtProfile.m_currentArt->second.m_palette = m_d3dSingleton->m_world->m_full->SetPalette();
		}

		/*void PersistentSingleton::DeepResetDevice()
		{

			if (static Tools::SingleCall onceResetApp; onceResetApp.test_and_set()) // Do not repeat if already running
				return;
			System::Shutdown();
			STARTUPINFO si{ .cb = sizeof STARTUPINFO };
			PROCESS_INFORMATION pi{};

			auto page = Pages::g_currentPage;
			if (L"unknown" == page || L"boot" == page)
				page.clear();
			else if (L"printing" == page)
				page = L"game";
			// Start the child process.
			ExecuteApp(
				Files::GetModulePath(),
				{
					{ System::g_pageNo, page },
					{ System::g_wallpaper, m_fileSearcher.FOLDER_BOOT2_WALLPAPER } });
		}*/

		void PersistentSingleton::ResetDevice()
		{
			OnDestroyDevice();
			OnCreateDevice();
		}

		void PersistentSingleton::SaveOnDestroy() noexcept
		{
			if (m_d3dSingleton && IsWM_CLOSE() && IsFull())
				SetPalette();
		}

		CombineCursorCrayon& PersistentSingleton::GetArrowCursor() noexcept
		{
			return m_d3dSingleton->m_world->m_full->m_arrowCursor;
		}

		void PersistentSingleton::OnReorganize()
		{
			if (const auto rect = GetMonitorRect(); GetPrimaryMonitor() != m_currentMonitor
				|| memcmp(&m_currentMonitorRect, &rect, sizeof(RECT)) != 0)
			{
				m_currentMonitor = GetPrimaryMonitor();
				m_currentMonitorRect = rect;

				ResetDevice();
			}
			ArrangeSatellites();
		}
		PersistentSingleton* g_singleton(nullptr);
#pragma endregion

#pragma region PersistentSingleton::CombineD3DSingleton
		PersistentSingleton::CombineD3DSingleton::CombineD3DSingleton()
		{
			g_samplers.SetAnisotropy(g_applied.m_quality + 1);
		}

		void PersistentSingleton::CombineD3DSingleton::Initialize()
		{
			m_world = make_unique<CombineWorld>();
			AddRenderedObject(m_world.get());
			FireEventFont();
		}

		void PersistentSingleton::CombineD3DSingleton::CreateFull()
		{
			m_world->CreateFull();
			FireEventFont();
		}
#pragma endregion

#pragma region PersistentSingleton::Config
		PersistentSingleton::Config::Config()
		{
			struct RegistryReadProfile : Registry
			{
				RegistryReadProfile(ConfigRecord& profile) :
					Registry{ KEY_READ }
				{
					profile = {
					.m_audio = GetBool(
						L"Audio",
						true
					),
					.m_printer = GetBool(
						L"Printer",
						true
					),
					.m_exit = GetBool(
						L"Exit",
						true
					),
					.m_vSync = GetBool(
						L"VSync",
						false
					),
					.m_quality = GetValidatedQuality(
						GetDWORD(
							L"Quality",
							RENDERER_QUALITY_MED
						)
					)
					};

					if (const std::string locale =
						GetString(
							L"Locale"
						); !locale.empty())
					{
						profile.m_locale = locale;
						SetLocaleName(
							profile.m_locale
						);
						g_singleton->InsertTask(&System::g_SetProfileFolder);
					}
				}
			} readProfile(m_before);
			RollbackChanges();
		}

		void PersistentSingleton::Config::ApplyChanges()
		{
			struct RegistryWriteProfile : Registry
			{
				inline RegistryWriteProfile() noexcept :
					Registry{ KEY_WRITE }
				{
					SetBool(
						L"Audio",
						g_applied.m_audio
					);
					SetBool(
						L"Printer",
						g_applied.m_printer
					);
					SetBool(
						L"Exit",
						g_applied.m_exit
					);
					SetBool(
						L"VSync",
						g_applied.m_vSync
					);
					SetDWORD(
						L"Quality",
						g_applied.m_quality
					);
					SetString(
						L"Locale",
						g_applied.m_locale
					);
				}
			} writeProfile;
			Pages::g_aAChanged = m_before.m_quality != g_applied.m_quality;
			const bool lang = m_before.m_locale != g_applied.m_locale;
			Pages::g_langChanged2 = lang && m_before.m_quality != g_applied.m_quality;
			Pages::g_langChanged = lang && m_before.m_quality == g_applied.m_quality;
			const bool audioChanged = m_before.m_audio != g_applied.m_audio;
			m_before = g_applied;
			if (audioChanged)
				Audio::g_persistentAudio->SetMuted();
			if (lang)
			{
				I18N::SetLocaleName(
					g_applied.m_locale
				);
				Win32ApplicationGeneric::SetTitle();
				g_singleton->m_satellitesManager.SetTitle();
				g_singleton->InsertTask(&System::g_SetProfileFolder);
			}
			if (Pages::g_langChanged && !Pages::g_aAChanged)
			{
				g_singleton->m_fileSearcher.SetPaths();
				g_singleton->InsertTaskSearcher2(LOAD_I18N_DESC);
			}
			ApplyVSync();
			if (Pages::g_aAChanged)
				g_singleton->ResetDevice();
			else
				Pages::SelectPage("main_menu"_pageNo);
		}

		void PersistentSingleton::Config::ApplyVSync() noexcept
		{
			m_vSync = g_applied.m_vSync;
		}

		void PersistentSingleton::Config::RollbackChanges() noexcept
		{
			static_cast<ConfigRecord&>(g_applied) = m_before;
		}

		bool PersistentSingleton::Config::IsChanged() const noexcept
		{
			return
				g_applied.m_audio != m_before.m_audio
				|| g_applied.m_printer != m_before.m_printer
				|| g_applied.m_exit != m_before.m_exit
				|| g_applied.m_vSync != m_before.m_vSync
				|| g_applied.m_quality != m_before.m_quality
				|| g_applied.m_locale != m_before.m_locale;
		}

		namespace
		{
			const wstring g_med = L"Med"s, g_hi = L"Hi"s, g_lo = L"Lo"s;
		}

		const wstring PersistentSingleton::Config::ConfigRecord::GetStringRendererQuality() const noexcept
		{
			switch (m_quality)
			{
			case RENDERER_QUALITY_MED: return g_med;
			case RENDERER_QUALITY_HI: return g_hi;
			default: return g_lo;
			}
		}
		PersistentSingleton::Config::ConfigRecordGlobal::ConfigRecordGlobal()
		{
			Monad::I18N::SetLocaleName(m_locale);
		}
#pragma endregion
	}
}