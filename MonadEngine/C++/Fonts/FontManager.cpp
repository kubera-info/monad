// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "FontManager.h"

// Monad
#include "GUI/Dialog.h"

namespace Monad::Fonts
{
	Text2Mesh& FontManager::GetFont(
		GUI::DialogGeneric& ownerDialog,
		const std::string& name
	)
	{
		const auto shaderFont = m_guiTexts.try_emplace(
			name,
			ownerDialog,
			name);
		if (shaderFont.second)
			ownerDialog.AddRenderedObject(&shaderFont.first->second);
		return shaderFont.first->second;
	}
}