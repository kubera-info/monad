// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Dialog.h"

// Monad
#include "Globals/__MonadGlobals.h"
#include "Fonts/FontBuilder.h"
#include "Kernel/Format.h"
#include "Frame.h"
#include "Pages/Page.h"
#include "I18N/I18N.h"

using namespace DirectX;

namespace Monad
{
	namespace GUI
	{
		using namespace Fonts;
		using namespace I18N;
		using namespace Kernel;
		using namespace Renderer;
		using namespace Repositories;

		namespace
		{
			constexpr XMFLOAT4 MAX_DIALOG{ -1.0f, -1.0f, 1.0f, 1.0f };
			constexpr float LEFT_PADDING = 0.04f;
			const I18NString HORIZONTAL_ELLIPSIS{ L"\x2026"_us };
		}
		DialogGeneric::DialogGeneric(
			const std::string& pageNo,
			const XMFLOAT4& dialogFrame,
			const I18N::I18NString& title
		) :
			c_pageNo{ pageNo },
			c_dlgRect{
				dialogFrame.x,
				dialogFrame.y * LANDSCAPE_RATIO,
				dialogFrame.z,
				dialogFrame.w * LANDSCAPE_RATIO
			},
			m_hr{ dialogFrame },
			m_border{ dialogFrame }
		{
			AddRenderedObjects({
				&m_hr,
				&m_border,
				&m_guiEmoji }
				);
			auto& text = GetFont(FONT_CENTURY_GOTHIC);
			text.AddRenderedText(
				Fonts::FONT_STYLE_OF_DIALOG_TITLE,
				XMFLOAT2{
					-dialogFrame.z + LEFT_PADDING,
					dialogFrame.w - 0.048f
				},
				title);
		}

		Text2Mesh& DialogGeneric::GetFont(
			const std::string& fontName
		)
		{
			return m_guiText.GetFont(
				*this,
				fontName
			);
		}

		DialogGeneric::HR::HR(const XMFLOAT4& dialogFrame) :
			ShaderMetallic{
			"metallic_hr"_technique,
			"hr"_mesh,
			Globals::g_hrColors, {
					abs(dialogFrame.x - dialogFrame.z) / 2.0f,
					dialogFrame.w - MOVE_HR,
					dialogFrame.x + abs(dialogFrame.x - dialogFrame.z) / 2.0f,
					abs(dialogFrame.x - dialogFrame.z)
		}
		}
		{}

		DialogGeneric::Border::Border(
			const XMFLOAT4& dialogFrame
		) :
			ShaderMetallic{
			"metallic_border"_technique,
			DialogBorderMesh(dialogFrame),
			Globals::g_borderColors,
			dialogFrame
		}
		{}

		XMFLOAT4 DialogGeneric::GetInternalRect() const noexcept
		{
			return {
				c_dlgRect.x + SCALE,
				c_dlgRect.y + SCALE,
				c_dlgRect.z - SCALE,
				c_dlgRect.w - MOVE_HR * LANDSCAPE_RATIO
			};
		}

		struct FontStyle2 : FontStyle
		{
			constexpr FontStyle2(const XMFLOAT3& color) noexcept :
				FontStyle{
				Fonts::FONT_STYLES::BOLD,
					{ 1.275f, 1.4f },
					{ color }
			}
			{}
		} const FONT_STYLE_CHECKBOX{
			{ .04f, .1f, .3f } },
			FONT_STYLE_RADIO_BUTTON{ { .28f, .1f, .17f }
		};

		void DialogGeneric::OnNewMatrix(
			XMFLOAT4X4& matrixToAlter
		) const noexcept
		{
			XMStoreFloat4x4(
				&matrixToAlter,
				XMMatrixMultiply(
					XMLoadFloat4x4(Renderer::g_dataMorphingMatrix),
					XMLoadFloat4x4(&matrixToAlter)));
		}

		void DialogGeneric::LabelTwoStatesButton(
			TwoStateButton& labeledTwoStatesButton,
			const I18N::I18NString& label,
			const Fonts::FontStyle& style
		)
		{
			AddRenderedObject(&labeledTwoStatesButton);
			GetFont(FONT_TIMES_NEW_ROMAN).
				AddRenderedText(
					style,
					labeledTwoStatesButton.GetPos(),
					label
				);
		}

		bool DialogGeneric::IsVisible() const noexcept
		{
			return c_pageNo == Pages::g_currentPage;
		}

		void DialogGeneric::LabelCheckbox(
			Checkbox& checkbox,
			const I18N::I18NString& label
		)
		{
			LabelTwoStatesButton(
				checkbox,
				label,
				FONT_STYLE_CHECKBOX
			);
		}

		void DialogGeneric::LabelRadioButton(
			ManagerRadioButton::RadioButton& radioButton,
			const I18N::I18NString& label
		)
		{
			LabelTwoStatesButton(
				radioButton,
				label,
				FONT_STYLE_RADIO_BUTTON
			);
		}

		DialogFullScreenGeneric::DialogFullScreenGeneric(
			const std::string& pageNo,
			const I18N::I18NString& title) :
			DialogGeneric{
				pageNo,
				MAX_DIALOG,
				title
			}
		{}

		DialogFullScreenGeneric::DialogFullScreenGeneric(
			const std::string& pageNo,
			const I18N::I18NString& title,
			const I18N::I18NString& message) :
			DialogFullScreenGeneric{
				pageNo,
				title
			}
		{
			auto& text = GetFont(FONT_CENTURY_GOTHIC);
			text.AddRenderedText(
				INNER_FONT_STYLE,
				std::make_optional<XMFLOAT2>(
					-1.0f + LEFT_PADDING,
					.75f
				),
				message);
			if (!message.GetVal().contains(L"?"))
				text.AddRenderedText(
					INNER_FONT_STYLE,
					{},
					HORIZONTAL_ELLIPSIS
				);
		}
	}
}