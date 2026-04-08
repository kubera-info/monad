// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Cursor.h"
// Platform
#include <windowsx.h>
// Monad
#include "Core/DXSample.h"
#include "Exceptions/Exceptions.h"

using namespace DirectX;

namespace Monad
{
	using namespace GUI;

	namespace Input
	{
		std::atomic<CURSOR_TYPES> g_cursorType = CURSOR_TYPES::HOUR_GLASS;
		::DirectX::XMFLOAT2 g_cursorPos;

		std::atomic_bool g_active = true;

		const auto
			CURSOR_WAIT = LoadCursor(NULL, IDC_WAIT),
			CURSOR_NO = LoadCursor(NULL, IDC_NO),
			CURSOR_ARROW = LoadCursor(NULL, IDC_ARROW);

		void CenterCursor() noexcept
		{
			if (g_active && !Monad::Exceptions::InAnyCloseReason())
			{
				const long long rightUInt = DXSample::GetWidth() / 2, bottomUInt = DXSample::GetHeight() / 2;

				POINT pt;
				GetCursorPos(&pt);
				if (pt.x != rightUInt || pt.y != bottomUInt)
					::SetCursorPos(static_cast<int>(rightUInt), static_cast<int>(bottomUInt));
				SetCursorPosGeneric(static_cast<int>(rightUInt), static_cast<int>(bottomUInt));
			}
		}

		namespace
		{
			XMFLOAT2 Screen2Pane(
				const long x,
				const long y
			) noexcept
			{
				const auto xFloat = static_cast<float>(x),
					yFloat = static_cast<float>(y),
					dx = static_cast<float>(DXSample::GetWidth()),
					dy = static_cast<float>(DXSample::GetHeight()
						);

				XMFLOAT2 vectorAspectRet;
				const XMFLOAT2 internalCursorPos{
					-2.0f * xFloat / dx + 1.0f,
					-2.0f * yFloat / dy + 1.0f
				};
				XMStoreFloat2(&vectorAspectRet,
					XMVectorMultiply(
						XMLoadFloat2(&internalCursorPos),
						XMLoadFloat2(&g_dxSample->m_matrices.m_monitorProps.m_cursorFactor)));

				return vectorAspectRet;
			}
		}

		void SetCursorPosGeneric(
			const long x,
			const long y
		) noexcept
		{
			g_cursorPos = Screen2Pane(x, y);
		}

		void SetCursorPos(
			const LPARAM lParam
		) noexcept
		{
			SetCursorPosGeneric(
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam)
			);
		}

#pragma region CursorControl
		void CursorControl::SetNewCurrentCursor(
			HCURSOR newCursor
		) noexcept
		{
			m_currentCursor = newCursor;
			SetCurrentCursor();
		}

		void CursorControl::SetCurrentCursor() const noexcept
		{
			SetCursor(m_currentCursor);
		}
#pragma endregion
	}
}