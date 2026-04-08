// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppGUILookAndFeel.h"
#include "GUI/Dialog.h"

namespace Monad::App
{
	/// <summary>
	/// Toolbox dialog displayed in the drawing view.
	/// Contains utility buttons such as clear, print, help and exit.
	/// </summary>
	struct DialogToolbox final : GUI::DialogGeneric
	{
		DialogToolbox(
			const DirectX::XMFLOAT4& dialogFrame,
			const I18N::I18NString& title
		);

	private:
		/// <summary>
		/// Base class for toolbar buttons used in the toolbox.
		/// Provides common visual and behavioral configuration.
		/// </summary>
		interface ButtonOneToolbar abstract : GUI::ButtonOneToolbarGeneric
		{
			ButtonOneToolbar(
				const DirectX::XMFLOAT2 & positionOfCenter,
				const GUI::IconicStyle & style
			);
		};

		/// <summary>
		/// Clears the current drawing and restores default colors.
		/// </summary>
		struct ButtonBlank final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_blankButton;

		/// <summary>
		/// Starts the printing process of the current artwork.
		/// Visible only when printing is enabled.
		/// </summary>
		struct ButtonPrint final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

			bool IsVisible() const noexcept final;

		private:
			void OnClick() final;
		} m_printButton;

		/// <summary>
		/// Displays contextual help related to the current screen.
		/// </summary>
		struct ButtonHelp final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_helpButton;

		/// <summary>
		/// Exits the drawing screen and returns to the main menu.
		/// </summary>
		struct ButtonExit final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_exitButton;
	};
}