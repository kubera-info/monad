// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

namespace Monad::App
{
	extern const std::wstring
		ART,
		XML_ATTR_R,
		XML_ATTR_G,
		XML_ATTR_B,
		COL;

	/// <summary>
	/// Loads the user's profile data from XML.
	/// </summary>
	/// <remarks>
	/// The profile contains stored color values for previously edited artworks.
	/// The data is parsed and applied to the internal palette structures.
	/// </remarks>
	/// <param name="xmlHolder">
	/// Input XML stream containing the saved profile data.
	/// </param>
	void LoadXMLProfile(
		IStream* const xmlHolder
	);
}