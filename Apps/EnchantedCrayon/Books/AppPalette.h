// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "GUI/Dialog.h"

namespace Monad::App
{
	/// <summary>
	/// Indicates whether a color is currently being selected or dragged.
	/// Used by the palette and cursor interaction logic.
	/// </summary>
	extern bool g_colorClicked;

	/// <summary>
	/// Color palette dialog.
	/// Displays a grid of selectable color buttons used for coloring.
	/// </summary>
	struct DialogPalette final : GUI::DialogGeneric
	{
		/// <summary>
		/// Creates a palette dialog with a defined layout and title.
		/// </summary>
		/// <param name="dialogFrame">Position and size of the dialog.</param>
		/// <param name="title">Localized title of the palette.</param>
		DialogPalette(
			const DirectX::XMFLOAT4& dialogFrame,
			const I18N::I18NString& title
		);

	private:
		/// <summary>
		/// Collection of color buttons displayed in the palette.
		/// Each button represents a selectable color.
		/// </summary>
		std::vector<std::unique_ptr<GUI::Button>> m_colors;
	};
}