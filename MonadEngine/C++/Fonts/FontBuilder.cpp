// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "FontBuilder.h"

// Monad
#include "FontMeta.h"
#include "GUI/Dialog.h"
#include "Kernel/Format.h"
#include "Repository/Repo.h"
#include "Tools/Convert.h"

using namespace std;

struct DXSample;

namespace Monad
{
	using namespace GUI;
	using namespace I18N;
	using namespace Kernel;
	using namespace Renderer;

	namespace Fonts
	{
		namespace
		{
			DirectX::XMFLOAT2 g_carriagePos;
			float g_carriageHorizontalPos;
		}

#pragma region Text2Mesh
		Text2Mesh::Text2Mesh(
			GUI::DialogGeneric& ownerDialog,
			const string& fontName
		) :
			TexGUI{
				"font"_technique,
				{ fontName },
				T1_FONT,
				{}
		},
			m_ownerDialog{ ownerDialog },
			c_fontMeta{ Repositories::g_repositoryGeneric->m_dialogFonts[fontName] }
		{
			Register();
		}

		void Text2Mesh::AddRenderedText(
			const FontStyle& style,
			const OptionalCarriage& origin,
			const I18NString& renderedText,
			const wstring parameters
		)
		{
			m_recorders.emplace_back(
				style,
				origin,
				renderedText
			);
			m_parameter = parameters;
		}

		inline void Text2Mesh::RenderText(
			const RenderingRecorderAtom& atom
		)
		{
			if (atom.m_origin)
			{
				g_carriagePos = *atom.m_origin;
				g_carriageHorizontalPos = g_carriagePos.x;
			}
			const wstring textToRender = atom.m_renderedText.VFormat(m_parameter);
			const float fontX = atom.m_style.c_fontSize.x,
				fontY = atom.m_style.c_fontSize.y * LANDSCAPE_RATIO,
				bold = (atom.m_style.c_style & BOLD) && c_fontMeta.m_styleInFont.m_boldFound ? CENTER : .0f,
				italic = (atom.m_style.c_style & ITALIC) && c_fontMeta.m_styleInFont.m_italicFound ? CENTER : .0f;
			m_textToRender.ReserveMemoryForMesh(
				m_textToRender.GetSizeOfVertices() + (textToRender.size() << 2),
				m_textToRender.GetSizeOfIndices() + textToRender.size() * 6
			);
			for (size_t charIndex = 0; charIndex < textToRender.size(); ++charIndex)
			{
				switch (wchar_t charID = textToRender[charIndex]; charID)
				{
					// New line
				case CHAR_LF:
					g_carriagePos = { g_carriageHorizontalPos, g_carriagePos.y - GetLinesHeight(atom.m_style.c_fontSize.y) };
					break;
					// Emoji resource
				case CHAR_HASH:
				{
					const auto indexOld = ++charIndex;
					for (; charIndex < textToRender.length() && textToRender[charIndex] != CHAR_HASH; ++charIndex);
					const float xEmo = c_fontMeta.m_lineHeight * fontX * 0.725f;
					m_ownerDialog.m_guiEmoji.AddEmoji(
						Tools::UnicodeToUTF8(textToRender.substr(indexOld, charIndex - indexOld)),
						{
							-g_carriagePos.x,
							g_carriagePos.y * LANDSCAPE_RATIO - fontY * 0.01f,
							-xEmo,0
							- c_fontMeta.m_lineHeight * fontY * 0.925f }
					);
					g_carriagePos.x += xEmo;
					break;
				}
				case CHAR_BACKSLASH:
					++charIndex;
					charID = textToRender[charIndex];
					[[fallthrough]];
				default:
					// Character (the horizontal space not added to the mesh)
					if (CHAR_SPC != charID)
					{
						const auto& chr = c_fontMeta(charID, atom.m_style.c_style);
						Renderer::InputLayout::VERTEXFONT a, b, c, d;

						a.m_position.x = d.m_position.x = -g_carriagePos.x - chr.c_offset.x * fontX;
						a.m_position.y = b.m_position.y = g_carriagePos.y * LANDSCAPE_RATIO - chr.c_offset.y * fontY;
						b.m_position.x = c.m_position.x = a.m_position.x - chr.c_chunk.z * fontX;
						c.m_position.y = d.m_position.y = a.m_position.y - chr.c_chunk.w * fontY;

						a.m_color = b.m_color = GUI_BLACK;
						c.m_color = d.m_color = atom.m_style.c_color;

						a.m_texture.x = d.m_texture.x = chr.c_chunk.x + bold;
						a.m_texture.y = b.m_texture.y = chr.c_chunk.y + italic;
						b.m_texture.x = c.m_texture.x = GetRight(chr.c_chunk) + bold;
						c.m_texture.y = d.m_texture.y = GetBottom(chr.c_chunk) + italic;

						m_textToRender.AddQuad(a, b, c, d);
					}
					g_carriagePos.x += c_fontMeta(charID, atom.m_style.c_style).CalculateDistance(textToRender[charIndex + 1]) * fontX;
				}
			}
		}

		float Text2Mesh::GetLinesHeight(
			float fontSize
		) const noexcept
		{
			const auto newLine = c_fontMeta.m_lineHeight * 1.02f * fontSize;
			assert(newLine > 0.0f);
			return newLine;
		}

		void Text2Mesh::Commit()
		{
			for (const auto& rec : m_recorders)
				RenderText(rec);
			m_mesh = m_textToRender.Commit();
			m_textToRender.ClearAndShrink();
		}

		bool Text2Mesh::OnLanguageChangeFontStub()
		{
			Commit();

			return true;
		}

		void Text2Mesh::OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		)
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			g_forecolorFontGeneric->SetMe(GetCurrentConfig());
		}

		void Text2Mesh::OnComputePerShadowFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		)
		{
			__super::OnComputePerShadowFrame(modelSpaceMatrix);

			GetCurrentTexture().SetMe(GetCurrentConfig(), c_startTexSlot);
		}
#pragma endregion

#pragma region ForecolorGeneric
		ForecolorGeneric::ForecolorGeneric(
			const std::string& counter
		) noexcept :
			CB::ForecolorInstance(counter),
			c_me(this, g_forecolorFontGeneric
			)
		{}
		ForecolorGeneric* g_forecolorFontGeneric = nullptr;
#pragma endregion
	}
}