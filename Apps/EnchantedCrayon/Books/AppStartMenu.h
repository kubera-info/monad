// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppArtPane.h"
#include "AppToolbox.h"
#include "Renderer/Twin.h"

namespace Monad::App
{
	struct DialogArtSelection final : GUI::DialogFullScreenGeneric
	{
		DialogArtSelection();

		/// <summary>
		/// Assigns a new artwork to both preview panes.
		/// </summary>
		/// <param name="art">Iterator pointing to the selected artwork</param>
		void SetArt(
			Repository::ConstArtIterator art
		);

	private:
		/// <summary>
		/// Updates the primary preview with the selected artwork.
		/// </summary>
		void SetArtPreview(
			Repository::ConstArtIterator art
		);

		/// <summary>
		/// Updates the secondary preview used during transition animation.
		/// </summary>
		void SetArtPreviewSecond(
			Repository::ConstArtIterator art
		);

		/// <summary>
		/// Sets the visible horizontal range of the primary preview.
		/// </summary>
		void SetHorizontalRangePreview(
			const double start,
			const double end
		);

		/// <summary>
		/// Sets the visible horizontal range of the secondary preview.
		/// </summary>
		void SetHorizontalRangePreviewSecond(
			const double start,
			const double end
		);

		using CombineArtPreviewArtPreview =
			CombineArtPreview<ShaderArtPreview>;

		using CombineArtPreviewArtPreviewSecond =
			CombineArtPreview<ShaderArtPreviewSecond>;

		/// <summary>
		/// Main artwork preview.
		/// </summary>
		CombineArtPreviewArtPreview m_artPreview;

		/// <summary>
		/// Secondary preview used for animated transitions.
		/// </summary>
		CombineArtPreviewArtPreviewSecond m_artPreviewSecond;

	public:
		/// <summary>
		/// Handles animated transitions between artworks.
		/// </summary>
		struct TimeLerpCurrentArt final :
			Time::TimeLerp,
			Kernel::EventD3DRenderStub
		{
			TimeLerpCurrentArt(
				DialogArtSelection& parent
			);

			/// <summary>
			/// Starts animation for switching to the previous artwork.
			/// </summary>
			void MoveLeftButtonClicked();

			/// <summary>
			/// Starts animation for switching to the next artwork.
			/// </summary>
			void MoveRightButtonClicked();

		private:
			bool OnD3DRenderStub() final;
			void OnStop() final;

			/// <summary>
			/// Parent dialog owning the previews.
			/// </summary>
			DialogArtSelection& m_parentDialog;
		} m_currentArt;

	private:
		/// <summary>
		/// Base class for toolbar buttons.
		/// </summary>
		interface ButtonOneToolbar abstract : GUI::ButtonOneToolbarGeneric
		{
			ButtonOneToolbar(
				const DirectX::XMFLOAT2 & positionOfCenter,
				const GUI::IconicStyle & style
			);
		};

		/// <summary>
		/// Base class for audio-related checkboxes.
		/// </summary>
		interface CheckboxAudioGeneric abstract : GUI::Checkbox
		{
			CheckboxAudioGeneric(
				const GUI::IconicStyle & style,
				bool& isSelected
			);
		};

		/// <summary>
		/// Checkbox enabling or disabling audio.
		/// </summary>
		struct CheckboxAudioButton final : CheckboxAudioGeneric
		{
			using CheckboxAudioGeneric::CheckboxAudioGeneric;

		private:
			bool IsVisible() const noexcept final;
			void OnClick() final;
		} m_ifAudioButton;

		/// <summary>
		/// Audio error indicator checkbox.
		/// </summary>
		struct CheckboxAudioError final : CheckboxAudioGeneric
		{
			CheckboxAudioError();

		private:
			bool IsVisible() const noexcept final;
			void OnClick() noexcept final;
		} m_ifAudioErrorButton;

		/// <summary>
		/// Button for navigating to the previous artwork.
		/// </summary>
		struct ButtonLeft final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_leftButton;

		/// <summary>
		/// Button for navigating to the next artwork.
		/// </summary>
		struct ButtonRight final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_rightButton;

		/// <summary>
		/// Confirms artwork selection.
		/// </summary>
		struct ButtonOK final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_okButton;

		/// <summary>
		/// Exits the application.
		/// </summary>
		struct ButtonExit final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			bool IsVisible() const noexcept final;
			void OnClick() final;
		} m_exitButton;

		/// <summary>
		/// Opens the settings dialog.
		/// </summary>
		struct ButtonSetup final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_setupButton;

		/// <summary>
		/// Opens the help screen.
		/// </summary>
		struct ButtonHelp final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_helpButton;
	};

	/// <summary>
	/// Dialog displayed when the user wants to exit the application.
	/// </summary>
	struct DialogExitApp final : GUI::DialogFullScreenGeneric
	{
		DialogExitApp();

	private:
		/// <summary>
		/// Base class for exit dialog buttons.
		/// </summary>
		interface ButtonOneToolbar abstract : GUI::ButtonOneToolbarGeneric
		{
			ButtonOneToolbar(
				const DirectX::XMFLOAT2 & positionOfCenter,
				const GUI::IconicStyle & style);
		};

		/// <summary>
		/// Confirms application shutdown.
		/// </summary>
		struct ButtonExitApp final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_exitAppButton;

		/// <summary>
		/// Returns to the main menu without exiting.
		/// </summary>
		struct ButtonReturnToApp final : ButtonOneToolbar
		{
			using ButtonOneToolbar::ButtonOneToolbar;

		private:
			void OnClick() final;
		} m_returnToAppButton;
	};
}