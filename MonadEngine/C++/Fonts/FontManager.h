// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "FontBuilder.h"

namespace Monad
{
	namespace GUI
	{
		struct DialogGeneric;
	}
	namespace Fonts
	{
		/// <summary>
		/// Lightweight font repository associated with a dialog object.
		/// </summary>
		struct FontManager
		{
			/// <summary>
			/// Returns the monad containing the mesh creator for the specified font name.
			/// </summary>
			/// <param name="name">Name of the font typeface.</param>
			/// <returns>Font monad</returns>
			Text2Mesh& GetFont(
				GUI::DialogGeneric& ownerDialog,
				const std::string& name
			);

		private:
			Kernel::UnorderedMapString<Text2Mesh> m_guiTexts;
		};
	}
}
