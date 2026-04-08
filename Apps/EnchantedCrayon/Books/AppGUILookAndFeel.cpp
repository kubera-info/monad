// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppGUILookAndFeel.h"
// Monad
#include "GUI/__MonadGUI.h"
#include "Kernel/Templates.h"
#include "Renderer/__MonadRenderer.h"

using namespace DirectX;

namespace Monad
{
	using namespace GUI;
	using namespace Kernel;
	using namespace Renderer;

	namespace App
	{
		// ---------------------------------------------------------------------
		// Predefined GUI styles
		// ---------------------------------------------------------------------

		const IconicStyle
			STYLE_CANCEL_BUTTON{
				GUI_RED,
				{ JINGLE_EXIT },
				{ "no"_shaderResource }
		},
			STYLE_EXIT_BUTTON{
				GUI_RED,
				{ JINGLE_EXIT },
				{ "exit"_shaderResource }
		},
			STYLE_OK_BUTTON{
				GUI_GREEN,
				{ JINGLE_FOCUS },
				{ "ok"_shaderResource }
		},
			STYLE_HELP_BUTTON{
				GUI_BLUE,
				{},
				{ "help"_shaderResource }
		},
			STYLE_SETTINGS_BUTTON{
				GUI_SETTINGS_COLOR,
				{ JINGLE_VOPROS },
				{ "setup"_shaderResource }
		},
			STYLE_PRINT_BUTTON{
				GUI_WHITE,
				{ JINGLE_PRINT },
				{ "print"_shaderResource }
		},
			STYLE_ABOUT_BUTTON{
				GUI_GREEN,
				{ JINGLE_VOPROS },
				{ "c"_shaderResource }
		},
			STYLE_DESC_BUTTON{
				GUI_BLUE,
				{ JINGLE_VOPROS },
				{ "i"_shaderResource }
		},
			STYLE_OPEN_SOURCE_BUTTON{
				GUI_WHITE,
				{ JINGLE_VOPROS },
				{ "opensource"_shaderResource }
		},
			STYLE_SPEAKER_ERROR_BUTTON{
				GUI_RED,
				{},
				{ "mute"_shaderResource }
		},
			STYLE_CHECKBOX{
				{
					"checked"_shaderResource,
					"dummy"_shaderResource
				}
		},
			STYLE_CHECKBOX_SPEAKER{
				{
					"speaker"_shaderResource,
					"mute"_shaderResource
				}
		},
			STYLE_RADIO_BUTTON{
				GUI_RADIO_BUTTON_COLOR,
				{
					"radio"_shaderResource,
					"dummy"_shaderResource
				}
		};
	}
}