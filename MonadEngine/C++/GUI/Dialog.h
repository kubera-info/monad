// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Fonts/EmojiManager.h"
#include "Fonts/FontManager.h"
#include "Button.h"
#include "Renderer/Metallic.h"

namespace Monad
{
	namespace GUI
	{
		constexpr float
			MOVE_HR = 0.195f,
			SCALE = 0.065f / 2.0f,
			SCALE_DIV2 = SCALE / 2.0f;

		/// <summary>
		/// Base dialog structure containing the minimal required elements.
		/// </summary>
		interface DialogGeneric abstract : Renderer::Combine
		{
			friend Fonts::Text2Mesh;
			friend ManagerRadioButton;

			explicit DialogGeneric(
				const std::string& pageNo,
				const DirectX::XMFLOAT4& dialogFrame,
				const I18N::I18NString& title
			);

		protected:
			Fonts::Text2Mesh& GetFont(
				const std::string& fontName
			);
			DirectX::XMFLOAT4 GetInternalRect() const noexcept;
			void LabelCheckbox(
				Checkbox& checkbox,
				const I18N::I18NString& label
			);
			void LabelRadioButton(
				ManagerRadioButton::RadioButton& radioButton,
				const I18N::I18NString& label
			);

			Fonts::EmojiManager m_guiEmoji;
			struct GridInfo
			{
				DirectX::XMFLOAT4 m_contentMargin;
				float m_contentOffset, m_contentSize, m_contentBlock;
			};

		private:
			void OnNewMatrix(
				DirectX::XMFLOAT4X4& matrixToAlter
			) const noexcept final;
			void LabelTwoStatesButton(
				TwoStateButton& labeledTwoStatesButton,
				const I18N::I18NString& label,
				const Fonts::FontStyle& style
			);
			bool IsVisible() const noexcept final;

			Fonts::FontManager m_guiText;
			const std::string c_pageNo;
			const DirectX::XMFLOAT4 c_dlgRect;
			struct Border final : Renderer::ShaderMetallic
			{
				Border(
					const DirectX::XMFLOAT4& dialogFrame
				);
			} m_border;
			struct HR final : Renderer::ShaderMetallic
			{
				HR(
					const DirectX::XMFLOAT4& dialogFrame
				);
			} m_hr;
		};
		struct DialogFullScreenGeneric : DialogGeneric
		{
			explicit DialogFullScreenGeneric(
				const std::string& pageNo,
				const I18N::I18NString& title
			);
			explicit DialogFullScreenGeneric(
				const std::string& pageNo,
				const I18N::I18NString& title,
				const I18N::I18NString& message
			);
		};
	}
}
