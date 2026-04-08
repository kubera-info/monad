// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppGUILookAndFeel.h"
#include "AppToolbox.h"

namespace Monad::App
{
	/// <summary>
	/// Main settings dialog displayed in full-screen mode.
	/// Contains configuration options, language selection,
	/// rendering quality and application-related actions.
	/// </summary>
	struct DialogSettings final : GUI::DialogFullScreenGeneric
	{
		DialogSettings(
			uint32_t& langID
		);

		/// <summary>
		/// Base class for toolbar buttons used in the settings dialog.
		/// </summary>
		struct ButtonOneToolbar : GUI::ButtonOneToolbarGeneric
		{
			ButtonOneToolbar(
				const DirectX::XMFLOAT2& positionOfCenter,
				const GUI::IconicStyle& style
			);
		};

		/// <summary>
		/// Checkbox used for simple on/off configuration flags.
		/// </summary>
		struct CheckboxIfSome : GUI::Checkbox
		{
			CheckboxIfSome(
				const float posInCol,
				bool& isSelected
			);
		} m_ifVSyncButton, m_ifPrivPrintButton, m_ifPrivExitButton;

		/// <summary>
		/// Radio button group responsible for selecting rendering quality.
		/// </summary>
		struct RadioButtonManager3DMode final : private GUI::ManagerRadioButton
		{
			RadioButtonManager3DMode(
				DialogGeneric& parent
			);

		private:
			RadioButton m_efficiencyRadio, m_mediumRadio, m_qualityRadio;
		} m_graphicsRadio;

		/// <summary>
		/// Language selection radio button group.
		/// </summary>
		struct RadioButtonManagerLang final : private GUI::ManagerRadioButton
		{
			RadioButtonManagerLang(
				DialogSettings& parent,
				uint32_t& currID
			);

		private:
			/// <summary>
			/// Single language selection button.
			/// </summary>
			struct RadioButtonFlag : RadioButton
			{
				RadioButtonFlag(
					uint32_t& currID,
					RadioButtonManagerLang& manager,
					const std::string& flag,
					const std::string& localeName
				);

				void OnClick() override;

			private:
				const std::string c_localeName;
			};

			uint32_t m_locale;
			std::array<RadioButtonFlag, 4> m_flags;
		} m_langRadio;

		/// <summary>
		/// Button used to close the settings dialog without applying changes.
		/// </summary>
		struct ButtonExit final : ButtonOneToolbar
		{
			ButtonExit();

		private:
			void OnClick() final;
		} m_exitButton;

		/// <summary>
		/// Button used to apply and save configuration changes.
		/// </summary>
		struct ButtonOK final : ButtonOneToolbar
		{
			ButtonOK();

		private:
			bool IsVisible() const noexcept final;
			void OnClick() noexcept final;
		} m_okButton;

		/// <summary>
		/// Opens the "About" dialog.
		/// </summary>
		struct ButtonAbout final : ButtonOneToolbar
		{
			ButtonAbout();

		private:
			void OnClick() final;
		} m_aboutButton;

		/// <summary>
		/// Opens the application description dialog.
		/// </summary>
		struct ButtonDesc final : ButtonOneToolbar
		{
			ButtonDesc();

		private:
			void OnClick() final;
		} m_descButton;

		/// <summary>
		/// Opens the open-source licenses dialog.
		/// </summary>
		struct ButtonOpenSource final : ButtonOneToolbar
		{
			ButtonOpenSource();

		private:
			void OnClick() final;
		} m_openSourceButton;
	};

	/// <summary>
	/// Base class for informational full-screen dialogs
	/// such as About, Description, or Open Source notices.
	/// </summary>
	interface DialogAboutGeneric abstract : GUI::DialogFullScreenGeneric
	{
		DialogAboutGeneric(
			const std::string & pageNo,
			const I18N::I18NString & title
		);

		/// <summary>
		/// OK button used to return to the previous screen.
		/// </summary>
		struct ButtonOK : GUI::ButtonOneToolbarGeneric
		{
			ButtonOK(
				const std::string& pageNo
			);

		private:
			void OnClick() final;
		} m_okButton;

	protected:
		/// <summary>
		/// Helper structure used for animated text rendering.
		/// Stores position and color fading state.
		/// </summary>
		struct PosPlusColor : DirectX::XMFLOAT2
		{
			PosPlusColor(
				DirectX::XMFLOAT2 pos,
				float foregroundColor,
				const float speed
			);

			inline void SubSpeed() noexcept;

			float m_foregroundColor;
			const float m_speed;
		};

		using InitializerListStrings =
			std::initializer_list<I18N::I18NString>;

		/// <summary>
		/// Draws multiple lines of text with animated coloring.
		/// </summary>
		void DrawDialogTexts(
			InitializerListStrings texts,
			const DirectX::XMFLOAT2 pos,
			const std::wstring& parameters = L""
		);

	private:
		/// <summary>
		/// Draws a single line of dialog text.
		/// </summary>
		void DrawDialogText(
			const I18N::I18NString& text,
			PosPlusColor& pos,
			const std::wstring& parameters = L""
		);
	};

	/// <summary>
	/// "About" dialog implementation.
	/// </summary>
	struct DialogAbout final : DialogAboutGeneric
	{
		DialogAbout();
	};

	/// <summary>
	/// Application description dialog.
	/// </summary>
	struct DialogDesc final : DialogAboutGeneric
	{
		DialogDesc();

		/// <summary>
		/// Animated application logo.
		/// </summary>
		struct LogoTexture final : Renderer::ShaderTextured<Renderer::ShaderNoLight>
		{
			LogoTexture();

		private:
			void OnNewMatrix(
				DirectX::XMFLOAT4X4& matrixToAlter
			) const noexcept final;
		} m_logo;
	};

	/// <summary>
	/// Open-source licenses and acknowledgements dialog.
	/// </summary>
	struct DialogOpenSource final : DialogAboutGeneric
	{
		DialogOpenSource();
	};
}