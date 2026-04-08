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
#include "AppParameter.h"
#include "HttpsAPI.h"
#include "BugXML.h"

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
		std::atomic_flag g_sent;

		namespace
		{
			inline RENDERER_QUALITIES GetValidatedQuality(
				const uint32_t
			) noexcept
			{
				return RENDERER_QUALITY_LO;
			}

			bool g_audio = true;
	
			Monad::Wnd::ExpectedLResult ProcessTASKBAR(
				HWND,
				UINT,
				WPARAM,
				LPARAM,
				LPVOID&
			)
			{
				g_singleton->ResetTaskbar();
				return Monad::Wnd::PROCESSING_OK;
			}
		}
#pragma region PersistentSingleton
		PersistentSingleton::PersistentSingleton() :
			c_me{
			this,
			g_singleton,
		},
		m_audioManager{ g_audio, { Audio::JINGLE } },
		m_threadPool{
			THREAD_NAME_SINGLETON_POOL,
			GetOptimalMapSize({
				{8U, 3U},
				{6U, 2U} },
				thread::hardware_concurrency(),
				2U)
		},
			c_msgTaskbarButtonCreated(
				System::PAR_VAL_INFO == g_parameters->GetParameter(System::PAR_NAME_TYPE)
				? WM_NULL
				: RegisterWindowMessage(L"TaskbarButtonCreated"))
		{
			if (WM_NULL != c_msgTaskbarButtonCreated)
			{
				g_forMainMessage.emplace(c_msgTaskbarButtonCreated, ProcessTASKBAR);
				ChangeWindowMessageFilterEx(
					Monad::Globals::g_mainWindow,
					c_msgTaskbarButtonCreated,
					MSGFLT_ALLOW,
					nullptr
				);
			}
			m_cursorControl.SetNewCurrentCursor(CURSOR_WAIT);
		}

		void PersistentSingleton::OnCreateDevice()
		{
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

			g_fileManagerState = App::FILE_MAN_STATES::STOP_BOOT_DX;
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

		bool PersistentSingleton::IsFull() const noexcept
		{
			return m_d3dSingleton->m_world && m_d3dSingleton->m_world->m_full;
		}

		void PersistentSingleton::ResetDevice()
		{
			OnDestroyDevice();
			OnCreateDevice();
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
		}

		void PersistentSingleton::Bulk()
		{
			Monad::Audio::g_persistentAudio->PlayJingle("yes"_cue);
			m_cursorControl.SetNewCurrentCursor(Monad::Input::CURSOR_WAIT);
			SetProgressState(TBPF_INDETERMINATE);
			InsertTask(&m_httpsTask);
		}

		void PersistentSingleton::OnThreadDone() noexcept
		{
			SetProgressState(
				IsReturnedXMLStatus().empty()
				? TBPF_ERROR
				: TBPF_NOPROGRESS);
			/*const auto buttonOk = GetDlgItem(IDOK);
			buttonOk->SetWindowText(FINISH.ToLPCWSTR());
			buttonOk->EnableWindow(TRUE);
			buttonOk->SetFocus();*/
			m_cursorControl.SetNewCurrentCursor(Monad::Input::CURSOR_ARROW);
		}
		PersistentSingleton::TaskHTTPS::TaskHTTPS() :
			ITask(false)
		{
		}
		void PersistentSingleton::TaskHTTPS::Run()
		{
			g_sent.test_and_set();
			struct SmartPostMessage
			{
				~SmartPostMessage()
				{
					Monad::Pages::SelectPage("exit"_pageNo);
				}
			} postMessageThreadDone;

			if (const auto httpsResponse = Monad::Internet::GetBugReportOnServer(); httpsResponse.has_value())
				const LoadXMLStatus xmlParseResponse(Monad::Kernel::SpanConstBytes(*httpsResponse).GetComIStreamTemp().Get());
		}
	
		void PersistentSingleton::SetProgressState(
			const TBPFLAG flag
		) noexcept
		{
			if (m_taskbarList)
			{
				m_taskbarList->SetProgressState(Monad::Globals::g_mainWindow, flag);
				if ((TBPF_ERROR & flag) > 0)
					m_taskbarList->SetProgressValue(Monad::Globals::g_mainWindow, 192, 256);
			}
		}
		void PersistentSingleton::ResetTaskbar() noexcept
		{
			m_taskbarList.Reset();
			if (c_msgTaskbarButtonCreated && SUCCEEDED(
				CoCreateInstance(
					CLSID_TaskbarList,
					nullptr,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(&m_taskbarList))))
				SetProgressState(TBPF_ERROR);
		}
		PersistentSingleton* g_singleton(nullptr);
#pragma endregion

#pragma region PersistentSingleton::CombineD3DSingleton
		PersistentSingleton::CombineD3DSingleton::CombineD3DSingleton()
		{
			g_samplers.SetAnisotropy(1);
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
	}
}