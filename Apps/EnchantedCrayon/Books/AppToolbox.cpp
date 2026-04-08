// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppToolbox.h"
// Monad
#include "AppGUILookAndFeel.h"
#include "AppSingleton.h"
#include "Printer/Sketch.h"
#include "Renderer/__MonadRenderer.h"

using namespace DirectX;

namespace Monad
{
	using namespace GUI;
	using namespace Kernel;
	using namespace Print;
	using namespace Renderer;

	namespace App
	{
		namespace
		{
			const IconicStyle STYLE_CLEAR_BUTTON{ GUI_GREEN, { JINGLE_CLEAR1, JINGLE_CLEAR2 }, { "new"_shaderResource } };
		}
#pragma region DialogToolbox
		DialogToolbox::DialogToolbox(
			const XMFLOAT4& dialogFrame,
			const I18N::I18NString& title
		) :
			DialogGeneric{
			"game"_pageNo,
			dialogFrame,
			title },
			m_blankButton{ {
					POS_X,
					POS_Y },
					STYLE_CLEAR_BUTTON
			},
			m_printButton{ {
					POS_X - DISTANCE,
					POS_Y },
					STYLE_PRINT_BUTTON
			},
			m_helpButton{ {
					POS_X - DISTANCE * 2.0f,
					POS_Y },
					STYLE_HELP_BUTTON
			},
			m_exitButton{ {
					POS_X - DISTANCE * 3.0f,
					POS_Y },
					STYLE_CANCEL_BUTTON
			}
		{
			AddRenderedObjects({
				&m_blankButton,
				&m_printButton,
				&m_helpButton,
				&m_exitButton
				}
			);
		}
#pragma endregion

#pragma region DialogToolbox::Button
		bool DialogToolbox::ButtonPrint::IsVisible() const noexcept
		{
			return g_singleton->m_config.m_before.m_printer; // || g_amIVisible;
		}

		DialogToolbox::ButtonOneToolbar::ButtonOneToolbar(
			const XMFLOAT2& positionOfCenter,
			const GUI::IconicStyle& style) :
			ButtonOneToolbarGeneric{
				"game"_pageNo,
				positionOfCenter,
				style }
		{}

		void DialogToolbox::ButtonBlank::OnClick()
		{
			g_colorClicked = false;
			g_singleton->m_d3dSingleton->m_world->ResetColors();
		}

		void DialogToolbox::ButtonPrint::OnClick()
		{
			g_colorClicked = false;
			g_singleton->SetPalette();
			Pages::SelectPage("printing"_pageNo);
			g_shaderSketchGeneric->PrintOnce();
		}

		void DialogToolbox::ButtonHelp::OnClick()
		{
			g_colorClicked = false;
			Audio::g_persistentAudio->Describe(
				g_combineFullRequirements->m_artPage.m_toolboxDialog.m_printButton.IsVisible()
				? "help_art_print"_cue : "help_art"_cue
			);
		}

		void DialogToolbox::ButtonExit::OnClick()
		{
			Pages::SelectPage("main_menu"_pageNo);

			g_singleton->m_d3dSingleton->m_world->FlushColors();
			g_repository->m_currentArtProfile.m_currentArt->second.ClearAndShrink();
			g_colorClicked = false;
			g_singleton->SetPalette();
		}
#pragma endregion
	}
}