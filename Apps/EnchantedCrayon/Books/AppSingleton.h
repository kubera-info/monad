// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "__App.h"
#include "I18N/I18N.h"
#include "Globals/__MonadGlobals.h"
#include "AppFileManager.h"
#include "AppWorld.h"
#include "Audio/Persistent.h"
#include "D3D12Framework.h"
#include "Files/__MonadFile.h"
#include "Fonts/FontMeta.h"
#include "Input/Cursor.h"
#include "Input/StickyKeys.h"
#include "AppKeyboardHook.h"
#include "Wnd/Monitor.h"
#include "Wnd/WindowSat.h"
#include "Renderer/Matrices.h"
#include "Renderer2D/2D.h"

namespace Monad::App
{
	/// <summary>
	/// Global application singleton.
	/// Exists for the entire lifetime of the application.
	/// </summary>
	struct PersistentSingleton final
	{
		PersistentSingleton();
		OPER_DEL_NO_DEF_CTOR(PersistentSingleton);

		void InsertTask(
			Threads::ThreadPool::ITask* const task
		);
		void InsertTaskSearcher();
		void InsertTaskSearcher2(
			const FILE_MAN_STATES newState
		);

		void OnCreateDevice();
		void CreateFull();
		void OnDestroyDevice() noexcept;
		void ArrangeSatellites();
		bool IsFull() const noexcept;
		void SetPalette() noexcept;
		void ResetDevice();
		void SaveOnDestroy() noexcept;
		void OnReorganize();

		CombineCursorCrayon& GetArrowCursor() noexcept;

	private:
		const Kernel::Me<PersistentSingleton> c_me;
		Input::StickyKeys m_stickyKeysDissabler;
		KeyboardHookNoWinKey m_noWinKey;

	public:
		HMONITOR m_currentMonitor = Monad::Wnd::GetPrimaryMonitor();
		RECT m_currentMonitorRect = Monad::Wnd::GetMonitorRect();

		Input::CursorControl m_cursorControl;

		const std::filesystem::path c_profilePath =
			GET_FOLDER_DOCUMENT / Files::PROFILE_FILENAME;

		Safety::SafetyState m_cipher;
		Renderer2D::Factory2D m_2D;
		Repository m_repository;
		TaskFileSearcher m_fileSearcher;
		Threads::ThreadPool m_threadPool;

		struct Config
		{
			Config();
			void ApplyChanges();
			void ApplyVSync() noexcept;
			void RollbackChanges() noexcept;
			bool IsChanged() const noexcept;

			struct ConfigRecord
			{
				const std::wstring GetStringRendererQuality() const noexcept;

				bool m_audio = true;
				bool m_printer = true;
				bool m_exit = true;
				bool m_vSync = false;
				uint32_t m_quality = 0;
				std::string m_locale = I18N::GetLocaleName();
			} m_before;

			struct ConfigRecordGlobal : ConfigRecord
			{
				ConfigRecordGlobal();
			};

			bool m_vSync = true;
		} m_config;

		/// <summary>
		/// Audio subsystem facade.
		/// </summary>
		Audio::PersistentAudio m_audioManager;

		/// <summary>
		/// Direct3D subsystem bound to the application lifetime.
		/// </summary>
		struct CombineD3DSingleton final : D3D12Core
		{
			CombineD3DSingleton();
			void Initialize();
			void CreateFull();

			std::unique_ptr<CombineWorld> m_world;
		};

		std::unique_ptr<CombineD3DSingleton> m_d3dSingleton;

		/// <summary>
		/// Manager responsible for satellite windows.
		/// </summary>
		Wnd::ManagerSatellites m_satellitesManager;
	}
	extern* g_singleton;

	extern PersistentSingleton::Config::ConfigRecordGlobal g_applied;
}