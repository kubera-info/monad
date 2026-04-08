// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppArtPane.h"
// Monad
#include "AppSingleton.h"
#include "AppConstantBuffers.h"
#include "Exceptions/Exceptions.h"
#include "Input/Cursor.h"
#include "Kernel/__MonadKernel.h"
#include "Kernel/Format.h"

using namespace DirectX;
using namespace std;

namespace Monad
{
	using namespace Exceptions;
	using namespace Input;
	using namespace Kernel;
	using namespace Renderer;
	using namespace Repositories;

	namespace App
	{
		namespace
		{
			constexpr float
				PREVIEW_HEIGHT = 0.3875f,
				PREVIEW_WIDTH = PREVIEW_HEIGHT * A4_LANDSCAPE_RATIO;
		}
#pragma region ShaderArtGeneric
		ShaderArtGeneric::ShaderArtGeneric(
			ArrayColors& colors,
			const string& palette,
			const XMFLOAT4& paneRect
		) :
			ShaderTextured{
				"raw_tex"_technique,
				{ GHOST },
				T2_ART,
				InputLayout::MeshBuilderArtPane{
					paneRect,
					0.0,
					1.0 }.Commit(false) },
					m_colors{ colors },
					m_paletteName{ palette },
					c_paneRect{ paneRect }
		{}

		void ShaderArtGeneric::SetArt(const Repository::ConstArtIterator art) noexcept
		{
			GetCurrentTexture() = g_repository->GetArtTexture(art);

			m_colors = art->second.m_palette;
		}

		void ShaderArtGeneric::OnComputePerWorldFrame(const XMFLOAT4X4& modelSpaceMatrix)
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			g_repository->m_shaderResources.GetMember(m_paletteName).SetMe(
				GetCurrentConfig(),
				T3_PALETTE);
		}
#pragma endregion

#pragma region ShaderArtPreview
		ShaderArtPreview::ShaderArtPreview(
			ArrayColors& colors,
			const string& palette
		) :
			ShaderArtGeneric{
				colors,
				palette,
				{
					-PREVIEW_WIDTH,
					-PREVIEW_HEIGHT,
					PREVIEW_WIDTH,
					PREVIEW_HEIGHT
				}
		}
		{}

		void ShaderArtPreview::SetHorizontalRange(
			const double start,
			const double end
		)
		{
			if (start != m_start || end != m_end)
			{
				m_start = start;
				m_end = end;

				m_mesh.SetBuffers(
					InputLayout::MeshBuilderArtPane{
						c_paneRect,
						m_start,
						m_end }.GetSettings(),
						g_d3d12Core->m_commandList);
			}
		}
#pragma endregion

#pragma region ShaderArtPreviewSecond
		bool ShaderArtPreviewSecond::IsVisible() const noexcept
		{
			return g_combineFullRequirements->m_artSelectionDialog.m_currentArt.IsRunning();
		}
#pragma endregion

#pragma region ShaderTex2Pane
		ShaderTex2Pane::ShaderTex2Pane(
			ArrayColors& colors,
			const string& palette,
			const XMFLOAT4& dialogFrame
		) :
			ShaderArtGeneric{
			colors,
			palette,
			dialogFrame
		},
			Clickable{
			"game"_pageNo,
			{
				"paint1"_cue,
				"paint2"_cue
			}
		},
			c_size{
			-1.0f / (dialogFrame.z - dialogFrame.x),
			-1.0f / (dialogFrame.w - dialogFrame.y) },
			c_offset{
				-dialogFrame.z,
				-dialogFrame.w }
		{
			EventPointerUpStub::Register();
		}

		void ShaderTex2Pane::OnClick() noexcept
		{
			g_singleton->m_d3dSingleton->m_world->SetNewColor(
				(g_cursorPos.x + c_offset.x) * c_size.x,
				(g_cursorPos.y + c_offset.y) * c_size.y);
		}

		bool ShaderTex2Pane::OnPointerUpStub()
		{
			if (IsInRegion() && g_colorClicked)
			{
				PlayClickJingle();
				OnClick();
				return false; // Found
			}
			return true;
		}

		bool ShaderTex2Pane::OnRegion() const noexcept
		{
			return between(
				c_paneRect.x,
				c_paneRect.z,
				g_cursorPos.x)
				&& between(
					c_paneRect.y,
					c_paneRect.w,
					g_cursorPos.y);
		}
#pragma endregion
	}
}