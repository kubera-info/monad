// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Page.h"

// Monad
#include "Core/Win32ApplicationGeneric.h"
#include "Audio/Persistent.h"
#include "Files/FileManager.h"
#include "Input/Cursor.h"
#include "Renderer/ShaderMatrix.h"
#include "System/Exit.h"

using namespace std;

namespace Monad
{
	using namespace Input;

	namespace Pages
	{
		bool g_langChanged2 = false, g_langChanged = false, g_aAChanged = false;
		string g_resetPageBackup = "main_menu"_pageNo;

		ScopeCounter::ScopeCounter() noexcept
		{
			++g_hourGlassBlockingRefs;
		}
		ScopeCounter::~ScopeCounter()
		{
			--g_hourGlassBlockingRefs;
		}

		const std::string& ResetPageBackup()
		{
			return g_resetPageBackup;
		}

#pragma region HourGlassCursorRefs
		void HourGlassCursorRefs::operator++() noexcept
		{
			m_liveRefs.fetch_add(1, memory_order_relaxed);
			Pages::SetMyCursor();
		}

		void HourGlassCursorRefs::operator--() noexcept
		{
			assert(m_liveRefs && "Cannot remove from empty");
			m_liveRefs.fetch_sub(1, memory_order_relaxed);
			Pages::SetMyCursor();
		}

		HourGlassCursorRefs::operator bool() const noexcept
		{
			return m_liveRefs != 0;
		}

		void HourGlassCursorRefs::Wait() const noexcept
		{
			while (operator bool())
				SwitchToThread();
		}
		HourGlassCursorRefs g_hourGlassBlockingRefs;

		atomic_bool g_processingPages = false;
#pragma endregion

		namespace
		{
			TimeLerpMorphing::GET_LERPS g_isYou = TimeLerpMorphing::GET_LERPS::RAISE;
		}

		void ResetCurrentPage() noexcept
		{
			SelectPage("boot"_pageNo);
		}

		void SetMyCursor() noexcept
		{
			g_cursorType = g_processingPages || "printing"_pageNo == g_currentPage
				|| Files::FILE_MAN_STATE_READY > Files::g_fileManagerState || g_hourGlassBlockingRefs ?
				CURSOR_TYPES::HOUR_GLASS :
				CURSOR_TYPES::POINTER;
		}

		void SelectPage(
			const string& pageNo
		)
		{
			if (g_MorphingDecay && pageNo != g_currentPage)
			{
				if ("boot"_pageNo != pageNo)
					g_resetPageBackup = pageNo;
				g_MorphingDecay->SelectPage(pageNo);
			}
		}

		void DequeuePage()
		{
			if (g_MorphingDecay)
				g_MorphingDecay->DequeuePage();
		}
#pragma region TimeLerpMorphing

		TimeLerpMorphing::TimeLerpMorphing(const double startValue, const double stopValue, const GET_LERPS isMe) noexcept :
			TimeLerp{
			.725,
			startValue,
			stopValue
		},
			c_isMe{ isMe }
		{}

		void TimeLerpMorphing::SetMorphing(float lerp) noexcept
		{
			XMStoreFloat4x4(
				Renderer::g_dataMorphingMatrix,
				DirectX::XMMatrixScaling(lerp, lerp, 1.0f)
			);
		}

		void TimeLerpMorphing::OnFrameMove() noexcept
		{
			if (c_isMe == g_isYou)
				SetMorphing(GetCurrentValue());
		}

		void TimeLerpMorphing::SetFinish() noexcept
		{
			SetMorphing(static_cast<float>(GetStopValue()));
		}
#pragma endregion

#pragma region TimeLerpMorphingDecay
		TimeLerpMorphingDecay::TimeLerpMorphingDecay() :
			TimeLerpMorphing{
			1.0,
			0.0,
			GET_LERPS::DECAY
		},
			c_me{ this, g_MorphingDecay },
			m_raise{
				0.0,
				1.0,
				GET_LERPS::RAISE
			}
		{}

		void TimeLerpMorphingDecay::OnFlush() noexcept
		{
			if (g_langChanged)
			{
				Kernel::FireEventFont();
			}
			g_aAChanged = false;
			g_currentPage = m_pageNo;
			if ("exit"_pageNo != m_pageNo)
			{
				g_isYou = GET_LERPS::RAISE;
				m_raise.Start();
				if ("printing"_pageNo != m_pageNo)
					Win32ApplicationGeneric::ActivateDirect3DWindow();
			}
			else
				System::Shutdown();
		}

		void TimeLerpMorphingDecay::OnFrameMove() noexcept
		{
			__super::OnFrameMove();
			if(IsInExit() && Audio::g_persistentAudio)
				Audio::g_persistentAudio->SetMasterVoiceVolume(GetCurrentValue());
		}

		inline void TimeLerpMorphingDecay::SetMorphedPage(
			const string& pageNo
		)
		{
			g_processingPages = true;
			if (Audio::g_persistentAudio)
				Audio::g_persistentAudio->ResetQueueDescribe();
			SetMyCursor();
			m_pageNo = pageNo;

			if ("boot"_pageNo != pageNo && !pageNo.starts_with("quick"_pageNo))
			{
				g_isYou = GET_LERPS::DECAY;
				Start();
			}
			else
				Flush();
		}

		bool TimeLerpMorphingDecay::IsWorking() const noexcept
		{
			return g_processingPages;
		}

		void TimeLerpMorphingDecay::DequeuePage()
		{
			if (!m_pageQueue.empty() && !IsWorking())
			{
				const auto desiredPage = m_pageQueue.front();
				m_pageQueue.pop();
				SelectPage(desiredPage);
			}
		}

		bool TimeLerpMorphingDecay::IsInExit() const noexcept
		{
			return "exit"_pageNo == m_pageNo;
		}

		void TimeLerpMorphingDecay::SelectPage(
			const string& pageNo
		)
		{
			if (IsWorking())
				m_pageQueue.push(pageNo);
			else
				SetMorphedPage(pageNo);
		}

		void TimeLerpMorphingDecay::TimeLerpRaise::OnFlush() noexcept
		{
			if (g_langChanged)
				g_langChanged = false;

			g_processingPages = false;
			SetMyCursor();
			SetFinish();
		}
		TimeLerpMorphingDecay* g_MorphingDecay = nullptr;
#pragma endregion
	}
}