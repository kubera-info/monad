// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppXMLProfile.h"
// Monad
#include "AppSingleton.h"
#include "Files/XML.h"
#include "Tools/Convert.h"

namespace Monad::App
{
	const std::wstring
		ART = L"Art"s,
		XML_ATTR_R = L"r"s,
		XML_ATTR_G = L"g"s,
		XML_ATTR_B = L"b"s,
		COL = L"Col"s;

	namespace
	{
		/// <summary>
		/// XML reader responsible for loading user's saved profile.
		/// </summary>
		struct XMLReaderProfile final : private Files::XMLManager
		{
			/// <summary>
			/// Parses XML and restores stored color palettes.
			/// </summary>
			/// <param name="xmlHolder">
			/// Stream containing XML data with saved artwork state.
			/// </param>
			XMLReaderProfile(
				IStream* const xmlHolder
			) :
				XMLManager{
				xmlHolder,
					{
					{ ART,
						[](const XMLElement& tag, XMLManager*)
					{
						const auto art = g_repository->m_memArts.find(
							Tools::UnicodeToUTF8(tag.GetIDAttr()));
						if (g_repository->m_memArts.cend() == art)
							return;

						auto& artPalette = art->second.m_palette;
						for (const auto& palette : tag.m_nestedElements)
						{
							const size_t index = std::stoull(palette.GetNameAttr());
							assert(artPalette.size() >= index);

							artPalette[index].AlterColor({
								palette.GetFloatAttr(XML_ATTR_R),
								palette.GetFloatAttr(XML_ATTR_G),
								palette.GetFloatAttr(XML_ATTR_B)
							});
						}
					} },
					}
			}
			{
				Processor();
			}
		};
	}

	/// <summary>
	/// Loads the user's profile from an XML stream.
	/// </summary>
	/// <param name="xmlHolder">
	/// Input XML stream containing serialized palette data.
	/// </param>
	void LoadXMLProfile(
		IStream* const xmlHolder
	)
	{
		const XMLReaderProfile loadStreamToXML(xmlHolder);
	}
}