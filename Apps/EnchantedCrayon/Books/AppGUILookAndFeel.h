// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "GUI/LookAndFeel.h"

namespace Monad::App
{
	/// <summary>
	/// Predefined GUI styles used throughout the application.
	/// These are shared, immutable style definitions.
	/// </summary>
	extern const GUI::IconicStyle
		STYLE_CANCEL_BUTTON,
		STYLE_EXIT_BUTTON,
		STYLE_OK_BUTTON,
		STYLE_HELP_BUTTON,
		STYLE_SETTINGS_BUTTON,
		STYLE_PRINT_BUTTON,
		STYLE_ABOUT_BUTTON,
		STYLE_DESC_BUTTON,
		STYLE_OPEN_SOURCE_BUTTON,
		STYLE_SPEAKER_ERROR_BUTTON,
		STYLE_CHECKBOX,
		STYLE_CHECKBOX_SPEAKER,
		STYLE_RADIO_BUTTON;
}