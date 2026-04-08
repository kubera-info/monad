// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "__MonadFont.h"

namespace Monad::Fonts
{
	void LoadXMLFont(
		IStream* const xmlHolder,
		const std::string& fontName
	);
}