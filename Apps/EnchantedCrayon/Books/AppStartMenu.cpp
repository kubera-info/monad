// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppStartMenu.h"
// Monad
#include "AppDict.h"
#include "AppGUILookAndFeel.h"
#include "AppSingleton.h"
#include "Pages/Page.h"
#include "Renderer/__MonadRenderer.h"

using namespace std;

namespace Monad
{
	using namespace GUI;
	using namespace I18N;
	using namespace Kernel;
	using namespace Pages;
	using namespace Renderer;

	namespace App
	{
		namespace
		{
			const IconicStyle
				STYLE_PAINT_BUTTON{ GUI_GREEN, { JINGLE_FOCUS }, { "wrist"_shaderResource } },
				STYLE_LEFT_BUTTON{ GUI_GREY, { JINGLE_CLEAR1, JINGLE_CLEAR2 }, { "left"_shaderResource } },
				STYLE_RIGHT_BUTTON{ GUI_GREY, { JINGLE_CLEAR1, JINGLE_CLEAR2 }, { "right"_shaderResource } };
			constexpr float g_period = 0.75f;
			bool s_true = true;
		}
#pragma region DialogArtSelection
		DialogArtSelection::DialogArtSelection() :
			DialogFullScreenGeneric(
				"main_menu"_pageNo,
				WELCOME),
			m_artPreview{
				make_unique<CombineArtPreviewArtPreview::FrameWithID>(g_combineFullRequirements->m_paletteMap.m_paletteContainer,NAME_PALETTE),
				make_unique<CombineArtPreviewArtPreview::FrameWithID1>(g_combineFullRequirements->m_paletteMap1.m_paletteContainer, NAME_PALETTE1) },
				m_artPreviewSecond{
					make_unique<CombineArtPreviewArtPreviewSecond::FrameWithID>(g_combineFullRequirements->m_secondaryPaletteMap.m_paletteContainer,NAME_SECONDARY_PALETTE),
					make_unique<CombineArtPreviewArtPreviewSecond::FrameWithID1>(g_combineFullRequirements->m_secondaryPaletteMap1.m_paletteContainer, NAME_SECONDARY_PALETTE1) },
					m_currentArt{ *this },
					m_ifAudioButton{
						STYLE_CHECKBOX_SPEAKER,
						g_applied.m_audio
					},
			m_leftButton{ {
					SIDE,
					0.0f },
				STYLE_LEFT_BUTTON
					},
			m_rightButton{ {
					-SIDE,
					0.0f },
					STYLE_RIGHT_BUTTON
					},
			m_okButton{ {
					0.0,
					POS_Y },
					STYLE_PAINT_BUTTON
					},
			m_exitButton{ {
					-SIDE,
					POS_Y },
					STYLE_EXIT_BUTTON
					},
			m_setupButton{ {
					-SIDE + 3.0f * DISTANCE,
					POS_Y },
					STYLE_SETTINGS_BUTTON
					},
			m_helpButton{ {
					-SIDE + DISTANCE,
					POS_Y },
					STYLE_HELP_BUTTON
					}
		{
			AddRenderedObjects({
				&m_leftButton,
				&m_rightButton,
				&m_okButton,
				&m_exitButton,
				&m_helpButton,
				&m_setupButton,
				&m_artPreview,
				&m_artPreviewSecond,
				&m_ifAudioButton,
				&m_ifAudioErrorButton
				}
			);
		}

		void DialogArtSelection::SetArt(
			Repository::ConstArtIterator art
		)
		{
			SetArtPreview(art);
			SetArtPreviewSecond(art);
		}

		void DialogArtSelection::SetArtPreview(
			Repository::ConstArtIterator art
		)
		{
			m_artPreview.SetArt(art);
		}

		void DialogArtSelection::SetArtPreviewSecond(
			Repository::ConstArtIterator art
		)
		{
			m_artPreviewSecond.SetArt(art);
		}

		void DialogArtSelection::SetHorizontalRangePreview(
			const double start,
			const double end)
		{
			m_artPreview.SetHorizontalRange(
				start,
				end
			);
		}

		void DialogArtSelection::SetHorizontalRangePreviewSecond(const double start, const double end)
		{
			m_artPreviewSecond.SetHorizontalRange(
				start,
				end
			);
		}
#pragma endregion

#pragma region DialogArtSelection::TimeLerpCurrentArt
		DialogArtSelection::TimeLerpCurrentArt::TimeLerpCurrentArt(
			DialogArtSelection& parent
		) :
			m_parentDialog{ parent },
			TimeLerp{ g_period }
		{
			EventD3DRenderStub::Register();
		}

		void DialogArtSelection::TimeLerpCurrentArt::MoveLeftButtonClicked()
		{
			m_parentDialog.SetArtPreviewSecond(g_repository->GetPreviousArt());
			StartWithParameters(1.0, 0.0);
		}

		void DialogArtSelection::TimeLerpCurrentArt::MoveRightButtonClicked()
		{
			m_parentDialog.SetArtPreviewSecond(g_repository->GetNextArt());
			StartWithParameters(0.0, 1.0);
		}

		bool DialogArtSelection::TimeLerpCurrentArt::OnD3DRenderStub()
		{
			if ("main_menu"_pageNo == Pages::g_currentPage
				&& Files::FILE_MAN_STATE_READY == Files::g_fileManagerState)
			{
				if (!IsFinished())
					if (GetStartValue() < GetStopValue())
					{
						m_parentDialog.SetHorizontalRangePreview(GetCurrentValue(), 1.0);
						m_parentDialog.SetHorizontalRangePreviewSecond(0.0, GetCurrentValue());
					}
					else
					{
						m_parentDialog.SetHorizontalRangePreview(0.0, GetCurrentValue());
						m_parentDialog.SetHorizontalRangePreviewSecond(GetCurrentValue(), 1.0);
					}
				else
					m_parentDialog.SetHorizontalRangePreview(
						0.0,
						1.0);
			}
			return true;
		}

		void DialogArtSelection::TimeLerpCurrentArt::OnStop()
		{
			const auto currentArt = g_repository->m_currentArtProfile.m_currentArt;
			THROW_EXC_ONEND(
				g_repository->m_memArts,
				currentArt,
				L"Stop Art");
			m_parentDialog.SetArtPreview(currentArt);
			g_singleton->
				m_d3dSingleton->
				m_world->m_full->
				m_artPage.m_pictureDialog.SetArt(currentArt);
		}
#pragma endregion

#pragma region DialogArtSelection::Button
		DialogArtSelection::ButtonOneToolbar::ButtonOneToolbar(
			const DirectX::XMFLOAT2& positionOfCenter,
			const GUI::IconicStyle& style
		) :
			ButtonOneToolbarGeneric{
				"main_menu"_pageNo,
				positionOfCenter,
				style }
		{}

		void DialogArtSelection::ButtonLeft::OnClick()
		{
			g_combineFullRequirements->m_artSelectionDialog.m_currentArt.MoveLeftButtonClicked();
		}

		void DialogArtSelection::ButtonRight::OnClick()
		{
			g_combineFullRequirements->m_artSelectionDialog.m_currentArt.MoveRightButtonClicked();
		}

		void DialogArtSelection::ButtonOK::OnClick()
		{
			const auto currentIt = g_repository->m_currentArtProfile.m_currentArt;
			THROW_EXC_ONEND(
				g_repository->m_memArts,
				currentIt,
				L"OnClick Set Art");
			SelectPage("game"_pageNo);
			g_singleton->InsertTask(&currentIt->second);
			g_combineFullRequirements->m_paletteMap.m_paletteContainer = currentIt->second.m_palette;
		}

		bool DialogArtSelection::ButtonExit::IsVisible() const noexcept
		{
			return g_singleton->m_config.m_before.m_exit; // || g_amIVisible;
		}

		void DialogArtSelection::ButtonExit::OnClick()
		{
			Pages::SelectPage("app_exit"_pageNo);
		}

		void DialogArtSelection::ButtonHelp::OnClick()
		{
			Audio::g_persistentAudio->Describe("help_menu");
		}

		DialogArtSelection::CheckboxAudioGeneric::CheckboxAudioGeneric(const GUI::IconicStyle& style, bool& isSelected) :
			Checkbox{
			"main_menu"_pageNo, {
					-SIDE + 2.0f * DISTANCE,
					POS_Y},
					style,
					isSelected
		}
		{}

		bool DialogArtSelection::CheckboxAudioButton::IsVisible() const noexcept
		{
			return __super::IsVisible() && Audio::g_persistentAudio->IsReady();
		}

		void DialogArtSelection::CheckboxAudioButton::OnClick()
		{
			Checkbox::OnClick();
			g_singleton->m_config.ApplyChanges();
		}

		DialogArtSelection::CheckboxAudioError::CheckboxAudioError() :
			CheckboxAudioGeneric(
				STYLE_SPEAKER_ERROR_BUTTON,
				s_true)
		{}

		bool DialogArtSelection::CheckboxAudioError::IsVisible() const noexcept
		{
			return __super::IsVisible() && !Audio::g_persistentAudio->IsReady();
		}

		void DialogArtSelection::CheckboxAudioError::OnClick() noexcept
		{}

		void DialogArtSelection::ButtonSetup::OnClick()
		{
			SelectPage("settings"_pageNo);
		}
#pragma endregion

		DialogExitApp::DialogExitApp() :
			DialogFullScreenGeneric(
				"app_exit"_pageNo,
				IF_EXIT,
				IF_EXIT_QUESTION),
			m_exitAppButton({
			-SIDE + DISTANCE,
			POS_Y },
			STYLE_EXIT_BUTTON
			),
			m_returnToAppButton({
			-SIDE,
			POS_Y },
			STYLE_PAINT_BUTTON
			)
		{
			AddRenderedObjects({
				&m_exitAppButton,
				&m_returnToAppButton
				}
			);
		}

		DialogExitApp::ButtonOneToolbar::ButtonOneToolbar(
			const DirectX::XMFLOAT2& positionOfCenter,
			const GUI::IconicStyle& style
		) :
			ButtonOneToolbarGeneric{
				"app_exit"_pageNo,
				positionOfCenter,
				style }
		{}

		void DialogExitApp::ButtonExitApp::OnClick()
		{
			Pages::SelectPage("exit"_pageNo);
		}

		void DialogExitApp::ButtonReturnToApp::OnClick()
		{
			Pages::SelectPage("main_menu"_pageNo);
		}
	}
}