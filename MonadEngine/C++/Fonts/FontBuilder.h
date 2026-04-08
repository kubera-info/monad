// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "__MonadFont.h"
#include "Core/DXSample.h"
#include "GUI/GUIMath.h"
#include "I18N/I18N.h"
#include "EmojiBuilder.h"
#include "Renderer/MeshCore.h"

namespace Monad
{
	namespace GUI
	{
		struct DialogGeneric;
	}
	namespace Fonts
	{
		struct FontStyle
		{
			const FONT_STYLES c_style;
			const DirectX::XMFLOAT2 c_fontSize;
			const DirectX::XMFLOAT3 c_color;
		};
		inline constexpr FontStyle
			FONT_STYLE_OF_DIALOG_TITLE{
			FONT_STYLES::BOLD,
				{ 1.5f, 1.75f },
				{ .025f, .025f, 1.0f }
		},
			INNER_FONT_STYLE{
			FONT_STYLES::ITALIC,
				{ 2.5f, 2.5f * 1.25f },
				{ .75f, 0.0f, .1f }
		},
			REPORT_FONT_STYLE{
			FONT_STYLES::NORMAL,
				{ 1.0f, 1.25f },
				{ .125f, .05f, .0f }
		};

		struct ForecolorGeneric final : Renderer::CB::ForecolorInstance
		{
			ForecolorGeneric(
				const std::string& counter
			) noexcept;

		private:
			const Kernel::Me<ForecolorGeneric> c_me;
		} extern* g_forecolorFontGeneric;

		using TexGUI = Renderer::ShaderTextured<Renderer::ShaderNoLight>;

		/// <summary>
		/// Class responsible for text rendering, generating a custom mesh using font data.
		/// </summary>
		struct Text2Mesh final : TexGUI, Kernel::EventLanguageChangeFontStub
		{
			using OptionalCarriage = std::optional<DirectX::XMFLOAT2>;
			Text2Mesh(
				GUI::DialogGeneric& ownerDialog,
				const std::string& fontName
			);
			void AddRenderedText(
				const FontStyle& style,
				const OptionalCarriage& origin,
				const I18N::I18NString& renderedText,
				const std::wstring parameters = L""
			);
			float GetLinesHeight(
				float fontSize
			) const noexcept;
			///<summary>
			/// Generates a combined mesh for all written text elements.
			/// </summary>
			void Commit();

		private:
			/// <summary>
			/// Regenerates font and emoji meshes to update text rendering.
			/// </summary>
			/// <returns></returns>
			bool OnLanguageChangeFontStub() final;
			void OnComputePerShadowFrame(
				const DirectX::XMFLOAT4X4& modelSpaceMatrix
			) final;
			void OnComputePerWorldFrame(
				const DirectX::XMFLOAT4X4& modelSpaceMatrix
			) final;
			struct RenderingRecorderAtom
			{
				const FontStyle m_style;
				const OptionalCarriage m_origin;
				const I18N::I18NString m_renderedText;
			};
			/// <summary>
			/// Adds a string for future rendering.
			/// </summary>
			/// <param name="atom">A simple text to render</param>
			inline void RenderText(
				const RenderingRecorderAtom& atom
			);

			GUI::DialogGeneric& m_ownerDialog;
			const Font& c_fontMeta;
			Renderer::InputLayout::MeshBuilderFont m_textToRender;
			std::vector<RenderingRecorderAtom> m_recorders;
			std::wstring m_parameter;
		};
	}
}
