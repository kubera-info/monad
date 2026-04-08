// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppCursor.h"
// Monad
#include "AppGesture.h"
#include "AppPalette.h"
#include "AppSingleton.h"
#include "D3D12Framework.h"
#include "Input/Cursor.h"
#include "Time/__MonadTime.h"

using namespace DirectX;

namespace Monad
{
	using namespace GUI;
	using namespace Input;
	using namespace Kernel;
	using namespace Renderer;
	using namespace Repositories;
	using namespace Time;

	namespace App
	{
		namespace
		{
			XMFLOAT3 g_newCrayonColor, g_previousCrayonColor;

			constexpr struct CB::PipeColor TORUS_COLORS
			{
				{ MND_MAKE_FLOAT(64), MND_MAKE_FLOAT(8), MND_MAKE_FLOAT(8), 1.0f },
				{ 1.0f, .25f, .25f, 1.0f }
			};

			constexpr double STATE_CHANGE = .05, UP = 0.01;
		}

		const XMFLOAT3& GetCrayonColor() noexcept
		{
			return g_newCrayonColor;
		}
#pragma region CombineCursorCrayon
		CombineCursorCrayon::CombineCursorCrayon() :
			m_timeLerpPushPop{
			STATE_CHANGE,
			UP,
			ART_PANE_Z
		},
			m_colorChange{
			*this
		}
		{
			const HDR::PowerFloat4 pink = GUI_PINK;
			InternalSetColor((g_previousCrayonColor = g_newCrayonColor = pink.GetFloatSRGB()));
			AddRenderedObjects({
				&m_paper,
				&m_crayon
				}
			);
		}

		void CombineCursorCrayon::InternalSetColor(
			const XMFLOAT3& cursorColor
		) noexcept
		{
			m_paper.SetColor(
				cursorColor
			);
			m_crayon.SetColor(
				cursorColor
			);
		}

		void CombineCursorCrayon::Push()
		{
			m_timeLerpPushPop.StartWithParameters(
				UP,
				ART_PANE_Z
			);
			g_amIInGesture = false;
			FireEvent(
				EVENT_IDS::POINTER_DOWN
			);
		}

		void CombineCursorCrayon::Pop()
		{
			// m_timeLerpPushPop.Flush();
			m_timeLerpPushPop.StartWithParameters(
				ART_PANE_Z,
				UP
			);
			FireEvent(
				EVENT_IDS::POINTER_UP
			);
			g_amIInGesture = g_colorClicked = false;
		}

		void CombineCursorCrayon::SetColor(
			const XMFLOAT3& cursorColor
		)
		{
			m_colorChange.Flush();
			g_newCrayonColor = cursorColor;
			m_colorChange.Start();
		}

		void CombineCursorCrayon::OnNewMatrix(
			XMFLOAT4X4& matrixToAlter
		) const noexcept
		{
			XMStoreFloat4x4(&matrixToAlter,
				XMMatrixMultiply(
					XMMatrixMultiply(XMLoadFloat4x4(&ROTATE_CURSOR),
						XMMatrixTranslation(
							g_cursorPos.x,
							g_cursorPos.y,
							m_timeLerpPushPop.GetCurrentValue())), XMLoadFloat4x4(
								&matrixToAlter
							)
				)
			);
		}

		bool CombineCursorCrayon::IsVisible() const noexcept
		{
			return CURSOR_TYPES::POINTER == g_cursorType;
		}
#pragma endregion

#pragma region CombineCursorCrayon::TimeLerpColorChange
		CombineCursorCrayon::TimeLerpColorChange::TimeLerpColorChange(
			CombineCursorCrayon& parent
		) noexcept :
			TimeLerp{
			.36,
			0.0,
			1.0
		},
			m_parent{
			parent
		}
		{}

		void CombineCursorCrayon::TimeLerpColorChange::OnFlush() noexcept
		{
			g_previousCrayonColor = g_newCrayonColor;
		}

		void CombineCursorCrayon::TimeLerpColorChange::OnFrameMove() noexcept
		{
			XMFLOAT3 resFloat3;
			XMStoreFloat3(
				&resFloat3,
				XMVectorLerp(
					XMLoadFloat3(&g_previousCrayonColor),
					XMLoadFloat3(&g_newCrayonColor),
					GetCurrentValue()));
			m_parent.InternalSetColor(resFloat3);
		}
#pragma endregion

#pragma region CombineCursorCrayon::ShaderCrayon
		inline CombineCursorCrayon::ShaderCrayon::ShaderCrayon() noexcept :
			ShaderMetallic2{
			"crayon"_technique,
			"crayon"_mesh }
		{}

		void CombineCursorCrayon::ShaderCrayon::SetColor(
			const XMFLOAT3& cursorColor
		) noexcept
		{
			m_rgb = cursorColor;
		}

		void CombineCursorCrayon::ShaderCrayon::OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		)
		{
			constexpr float spec = 2.0f;
#define SPEC(X) ((X) * spec + spec)
			m_cBufferMetallic.GetBuffer() =
			{
				.m_diffuseColor{ m_rgb.x, m_rgb.y, m_rgb.z, 1.0f },
				.m_color{ SPEC(m_rgb.x), SPEC(m_rgb.y), SPEC(m_rgb.z), 1.0f }
			};

			__super::OnComputePerWorldFrame(modelSpaceMatrix);
		}
#pragma endregion

#pragma region CombineCursorCrayon::ShaderPaper
		inline CombineCursorCrayon::ShaderPaper::ShaderPaper() :
			Shader{
				"paper"_technique,
				"paper"_mesh },
				m_forecolor{ "forecolor"_constantBuffer }
		{}

		void CombineCursorCrayon::ShaderPaper::SetColor(
			const XMFLOAT3& cursorColor
		) noexcept
		{
			m_rgb = cursorColor;
		}

		void CombineCursorCrayon::ShaderPaper::OnComputePerWorldFrame(
			const XMFLOAT4X4& modelSpaceMatrix
		)
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			constexpr float lum = 1.5f;
			XMStoreFloat3(&m_forecolor.GetBuffer(), XMVectorScale(
				XMLoadFloat3(&m_rgb),
				lum));
			m_forecolor.SetMe(GetCurrentConfig());
		}
#pragma endregion

#pragma region ShaderCursorTorus
		ShaderCursorTorus::ShaderCursorTorus() :
			ShaderMetallic2{
			"crayon"_technique,
			"torus"_mesh,
			TORUS_COLORS
		}
		{}

		void ShaderCursorTorus::OnNewMatrix(XMFLOAT4X4& matrixToAlter) const noexcept
		{
			XMStoreFloat4x4(&matrixToAlter, XMMatrixMultiply(XMMatrixMultiply(
				XMMatrixMultiply(
					XMMatrixRotationY(static_cast<float>(g_fTime) * 4.0f),
					XMMatrixRotationZ(static_cast<float>(g_fTime) * 2.0f)),
				XMMatrixTranslation(
					g_cursorPos.x,
					g_cursorPos.y,
					0.075f)), XMLoadFloat4x4(&matrixToAlter)));
		}

		bool ShaderCursorTorus::IsVisible() const noexcept
		{
			return CURSOR_TYPES::HOUR_GLASS == g_cursorType;
		}
#pragma endregion
	}
}