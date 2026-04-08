// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
// Monad
#include "I18N/I18N.h"

namespace Monad::GUI
{
	constexpr float
		DISTANCE = 0.114624f,
		POS_Y = -0.47625f,
		POS_X = -0.557755f,
		SIDE = -POS_X + DISTANCE * 3.0f,
		ART_PANE_Z = 0.0001f,
		ICON_RADIUS = 0.0442681f, // GridInfo.m_contentSize * Radius
		CENTER = .5f,
		BRIGHNESS_90 = 0.90f,
		BRIGHNESS_75 = 0.75f,
		BRIGHNESS_66 = 0.6666f,
		BRIGHNESS_50 = 0.5f,
		BRIGHNESS_33 = 0.3333f,
		BRIGHNESS_25 = 0.25f;

	constexpr DirectX::XMFLOAT3
		GUI_BLACK{ 0.0f, 0.0f, 0.0f },
		GUI_PINK{ 1.0f, 0.666f, 0.666f },
		GUI_WHITE{ 1.0f, 1.0f, 1.0f },
		GUI_GREEN{ 0.5f, 1.0f, 0.5f },
		GUI_GREY{ 0.75f, 0.75f, 0.75f },
		GUI_RED{ 1.0f, 0.125f, 0.125f },
		GUI_BLUE{ 0.5f, 0.5f, 1.0f },
		GUI_CHECKBOX_COLOR{ 0.0f, 0.5f, 1.0f },
		GUI_RADIO_BUTTON_COLOR{ 0.0f, 1.0f, 0.5f },
		GUI_SETTINGS_COLOR{ 1.0f, 1.0f, 0.5f };

	extern const std::string
		JINGLE_CLEAR1,
		JINGLE_CLEAR2,
		JINGLE_EXIT,
		JINGLE_NO,
		JINGLE_VOPROS,
		JINGLE_FOCUS,
		JINGLE_PRINT;


	/// <summary>
	/// Strips a leading emoji icon from the beginning of the title string.
	/// </summary>
	/// <param name="title">Title string that includes a leading emoji icon.</param>
	/// <returns>Text content after removing the leading emoji.</returns>
	LPCWSTR RemoveEmojiIcon(
		const I18N::I18NString& title
	) noexcept;
}
