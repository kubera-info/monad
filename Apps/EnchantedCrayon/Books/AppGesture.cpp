// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppGesture.h"
// Monad
#include "AppSingleton.h"
#include "Input/Cursor.h"

using namespace std;

namespace Monad
{
	using namespace Audio;
	using namespace Input;
	using namespace Kernel;

	namespace App
	{
		bool g_amIInGesture = false;
		Swipe::Swipe() :
			c_clicksSounds{
				"clear1"_cue,
				"clear2"_cue
		}
		{
			EventPointerDownStub::Register();
			EventPointerUpStub::Register();
		}

		bool Swipe::OnFrameMoveStub() noexcept
		{
			if (g_amIInGesture)
			{
				m_maxX = max(g_cursorPos.x, m_maxX);
				m_maxY = max(g_cursorPos.y, m_maxY);
				m_minX = min(g_cursorPos.x, m_minX);
				m_minY = min(g_cursorPos.y, m_minY);
			}

			return true;
		}

		bool Swipe::OnPointerDownStub() noexcept
		{
			if ("main_menu"_pageNo == Pages::g_currentPage)
			{
				g_amIInGesture = true;
				m_maxX = m_minX = m_startX = g_cursorPos.x;
				m_maxY = m_minY = g_cursorPos.y;
				EventFrameMoveStub::Register();

				return false;
			}
			else
				return true;
		}

		bool Swipe::OnPointerUpStub()
		{
			if (g_amIInGesture)
			{
				EventFrameMoveStub::Unregister();
				if (abs(m_maxY - m_minY) < 0.25f && abs(m_maxX - m_minX) > 0.275f)
				{
					if (abs(m_maxX - m_startX) > abs(m_minX - m_startX))
						g_combineFullRequirements->m_artSelectionDialog.m_currentArt.MoveRightButtonClicked();
					else
						g_combineFullRequirements->m_artSelectionDialog.m_currentArt.MoveLeftButtonClicked();
					c_clicksSounds.PlayJingle();
				}
				return false;
			}
			return true;
		}

		void IsMouseDownStub(
			const bool leftButtonDown
		)
		{
			if (CURSOR_TYPES::POINTER == g_cursorType
				&& !Exceptions::InAnyCloseReason()
				&& Files::FILE_MAN_STATE_READY == Files::g_fileManagerState)
				if (leftButtonDown)
					g_singleton->GetArrowCursor().Push();
				else
					g_singleton->GetArrowCursor().Pop();
		}
	}
}