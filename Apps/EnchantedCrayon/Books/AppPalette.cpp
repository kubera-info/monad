// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppPalette.h"
// Monad
#include "AppGUILookAndFeel.h"
#include "AppSingleton.h"
#include "Renderer/HDR.h"

using namespace DirectX;
using namespace std;

namespace Monad
{
	using namespace GUI;
	using namespace Renderer;
	using namespace Renderer::HDR;

	namespace App
	{
		bool g_colorClicked = false;

#pragma region DialogPalette
		DialogPalette::DialogPalette(
			const XMFLOAT4& dialogFrame,
			const I18N::I18NString& title
		) :
			DialogGeneric{
			"game"_pageNo,
			dialogFrame,
			title
		}
		{
			constexpr size_t
				SHADES_COUNT(4),
				COLORS_COUNT(7);
			constexpr auto
				CONTENT_COUNT_IN_ROW = static_cast<float>(SHADES_COUNT),
				CONTENT_COUNT_IN_COL = static_cast<float>(COLORS_COUNT);
			const XMFLOAT3 colors[SHADES_COUNT][COLORS_COUNT]
			{
				{ GUI_PINK, { 1.00f, 0.00f, 1.00f }, { BRIGHNESS_66, BRIGHNESS_66, 1.0f}, {0.00f, 1.00f, 1.00f}, {0.00f, 1.00f, 0.00f}, {1.00f, BRIGHNESS_90, 0.00f}, GUI_WHITE},
				{ { 1.00f, 0.00f, 0.00f }, { BRIGHNESS_75, 0.00f, BRIGHNESS_75 }, { 0.00f, 0.00f, 1.0f }, { 0.00f, BRIGHNESS_75, BRIGHNESS_75 }, { 0.00f, BRIGHNESS_75, 0.00f }, { BRIGHNESS_90, BRIGHNESS_75, 0.00f }, { BRIGHNESS_66, BRIGHNESS_66, BRIGHNESS_66 } },
				{ { BRIGHNESS_66, 0.00f, 0.00f }, { BRIGHNESS_50, 0.00f, BRIGHNESS_50 }, { 0.00f, 0.00f, BRIGHNESS_66 }, { 0.00f, BRIGHNESS_50, BRIGHNESS_50 }, { 0.00f, BRIGHNESS_50, 0.00f }, { BRIGHNESS_75, BRIGHNESS_50, 0.00f }, { BRIGHNESS_33, BRIGHNESS_33, BRIGHNESS_33 } },
				{ { BRIGHNESS_33, 0.00f, 0.00f }, { BRIGHNESS_25, 0.00f, BRIGHNESS_25 }, { 0.00f, 0.00f, BRIGHNESS_33 }, { 0.00f, BRIGHNESS_25, BRIGHNESS_25 }, { 0.00f, BRIGHNESS_25, 0.00f }, { BRIGHNESS_50, BRIGHNESS_25, 0.00f }, GUI_BLACK }
			};
			const GridInfo gridInfo{
			.m_contentMargin = GetInternalRect(),
			.m_contentOffset = gridInfo.m_contentMargin.z - 2.0f * SCALE,
			.m_contentSize = (gridInfo.m_contentMargin.z - gridInfo.m_contentMargin.x - SCALE_DIV2 * (CONTENT_COUNT_IN_ROW + 1.0f)) / CONTENT_COUNT_IN_ROW,
			.m_contentBlock = gridInfo.m_contentSize + SCALE_DIV2 };
			const auto contentOffsetCol = gridInfo.m_contentMargin.w - 0.07f,
				contentSizeCol = 0.975f * (gridInfo.m_contentMargin.w - gridInfo.m_contentMargin.y - SCALE_DIV2 * (CONTENT_COUNT_IN_COL + 1.0f)) / CONTENT_COUNT_IN_COL,
				contentBlockCol = contentSizeCol + SCALE_DIV2;

			/// <summary>
			/// A color button from a palette window
			/// </summary>
			struct ButtonColorSelector : Button
			{
				inline ButtonColorSelector(
					const XMFLOAT4& positionOfCenter,
					const XMFLOAT3& rgb
				) :
					Button{
						ForceConstForecolor,
						"game"_pageNo,
						positionOfCenter,
						{ONE_COLOR_STYLE,
						rgb,
						{ "color1"_cue, "color3"_cue, "color5"_cue },
						{}
				}
				}
				{}

			private:
				void OnClick()
				{
					g_colorClicked = true;
					g_singleton->GetArrowCursor().SetColor(m_forecolor.GetBuffer());
				}
			};

			m_colors.reserve(SHADES_COUNT * COLORS_COUNT);
			for (size_t shadesInd = 0; shadesInd < SHADES_COUNT; ++shadesInd)
				for (size_t colorsInd = 0; colorsInd < COLORS_COUNT; ++colorsInd)
					AddRenderedObject(m_colors.emplace_back(
						make_unique<ButtonColorSelector>(
							XMFLOAT4{
								gridInfo.m_contentOffset - gridInfo.m_contentBlock * shadesInd,
								contentOffsetCol - contentBlockCol * colorsInd,
								gridInfo.m_contentSize * CENTER, contentSizeCol * CENTER },
								PowerFloat4(colors[shadesInd][colorsInd]).GetFloatSRGB())).get());
		}
#pragma endregion
	}
}