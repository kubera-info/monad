// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "FontLoader.h"

// STD
#include <ranges>

// Monad
#include "__MonadFont.h"
#include "FontMeta.h"
#include "Files/XML.h"
#include "GUI/__MonadGUI.h"
#include "GUI/GUIMath.h"
#include "FontBuilder.h"

using namespace std;
using namespace DirectX;

namespace Monad::Fonts
{
	using namespace GUI;

	namespace
	{
		const wstring COUNT =
			L"count"s;

		const wstring SCALEH = L"scaleH"s,
			SCALEW = L"scaleW"s,
			LINE_HEIGHT = L"lnH"s;

		const wstring
			X = L"x"s,
			Y = L"y"s,
			WIDTH = L"w"s,
			HEIGHT = L"h"s,
			XOFFSET = L"xofs"s,
			YOFFSET = L"yofs"s,
			XADV = L"adv"s;

		const wstring
			FIRST = L"frst"s,
			SECOND = L"sec"s,
			AMOUNT = L"amt"s;

		const wstring
			BOLD_ATTR = L"b"s,
			ITALIC_ATTR = L"i"s;

		constexpr FONT_STYLES ToStyle(bool bold, bool italic) noexcept
		{
			size_t style = NORMAL;
			if (bold)
				style |= BOLD;
			if (italic)
				style |= ITALIC;

			return static_cast<FONT_STYLES>(style);
		}

		inline Fonts::Font::Char MakeCharacter(
			const XMFLOAT2 scale,
			const XMFLOAT4& chunk,
			const XMFLOAT2 offset,
			float advance
		) noexcept
		{
			return
			{
				{ chunk.x * scale.x, chunk.y * scale.y, chunk.z * scale.x, chunk.w * scale.y },
				{ offset.x * scale.x, offset.y * scale.y },
				advance * scale.x
			};
		}

#define XML_FONT_READER static_cast<XMLReaderFont*>(manager)->m_parsedFontInfo

		struct XMLReaderFont final : private Files::XMLManager
		{
			const StyleInFont GetStyleInFont()
			{
				assert(m_rootOfDoc.m_nestedElements.size());
				const auto& inf =
					m_rootOfDoc.m_nestedElements[m_rootOfDoc.m_nestedElements.size() - 1].
					m_nestedElements[0];
				return { inf.GetBoolAttr(ITALIC_ATTR), inf.GetBoolAttr(BOLD_ATTR) };
			}

			XMLReaderFont(
				IStream* xmlHolder,
				const string& fontName
			) :
				XMLManager{
				xmlHolder,
					{
			#pragma region Angel Code BM Font
				{ L"fnt",
				[](const XMLElement& tag, XMLManager* manager)
				{
					assert(manager);
					XMFLOAT2 fontStyleOffset{ .0f, .0f };
					FONT_STYLES style = FONT_STYLES::NORMAL;
					XMFLOAT2 scale{ 1.0f, 1.0f };

					for (const auto& descendant : tag.m_nestedElements)
						if (descendant.GetElementName() == L"cmn")
						{
							const auto scaleH = descendant.GetFloatAttr(SCALEH);
							scale = {
								(XML_FONT_READER.m_styleInFont.m_boldFound ? CENTER : 1.0f) / descendant.GetFloatAttr(SCALEW),
								(XML_FONT_READER.m_styleInFont.m_italicFound ? CENTER : 1.0f) / scaleH };
							XML_FONT_READER.SetFontScale(CENTER * descendant.GetFloatAttr(LINE_HEIGHT) / scaleH);
						}
						else if (descendant.GetElementName() == L"inf")
						{
							bool bold = false, italic = false;
							if (descendant.GetBoolAttr(BOLD_ATTR))
							{
								bold = true;
								fontStyleOffset.x = CENTER;
							}
							if (descendant.GetBoolAttr(ITALIC_ATTR))
							{
								italic = true;
								fontStyleOffset.y = CENTER;
							}
							style = ToStyle(bold, italic);
						}
						else if (descendant.GetElementName() == L"cs")
						{
							XML_FONT_READER.m_metaChars[style].insert_range(
								descendant.m_nestedElements |
								views::transform(
									[&](const XMLManager::XMLElement& charElement)
									{
										return
											make_pair(charElement.GetCharAttr(Files::XML_ATTR_ID),
												MakeCharacter(
													scale,
													{
														charElement.GetFloatAttr(X) + fontStyleOffset.x,
														charElement.GetFloatAttr(Y) + fontStyleOffset.y,
														charElement.GetFloatAttr(WIDTH),
														charElement.GetFloatAttr(HEIGHT)
													},
													{
														charElement.GetFloatAttr(XOFFSET),
														charElement.GetFloatAttr(YOFFSET)
													},
													charElement.GetFloatAttr(XADV)));
									}
								));
						}
						else if (descendant.GetElementName() == L"ks")
						{
							auto previousID{ CHAR_NUL }; // This character is not allowed
							Font::Char::MapKernings* charKernings(nullptr);
							for (const auto& kernings : descendant.m_nestedElements)
							{
								const auto current_id = kernings.GetCharAttr(FIRST);
								assert(CHAR_NUL != current_id);
								if (previousID != current_id)
									charKernings = &XML_FONT_READER(
										(previousID = current_id), style).m_kernings;
								charKernings->emplace(
									kernings.GetCharAttr(SECOND),
									kernings.GetFloatAttr(AMOUNT) * scale.x
								);
							}
						}
					}
				}
					} }, m_parsedFontInfo{ Repositories::g_repositoryGeneric->m_dialogFonts.try_emplace(
						fontName,
						Font{}).first->second }
#pragma endregion
			{
				m_parsedFontInfo.m_styleInFont = GetStyleInFont();
				Processor();
			}

		private:
			Font& m_parsedFontInfo;
		};
	}

	void LoadXMLFont(
		IStream* const xmlHolder,
		const string& fontName
	)
	{
		XMLReaderFont{
			xmlHolder,
			fontName
		};
	}
}
