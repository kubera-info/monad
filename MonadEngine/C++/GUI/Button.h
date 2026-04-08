// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <variant>

// Monad
#include "GUI.h"
#include "LookAndFeel.h"
#include "I18N/I18N.h"
#include "Renderer/ShaderMatrix.h"

namespace Monad::GUI
{
	struct ForceConstForecolorType
	{} extern ForceConstForecolor;

	/// <summary>
	/// Abstract base class representing a generic button.
	/// </summary>
	interface Button abstract : Pane, Clickable
	{
		Button(
			const std::string & pageNo,
			const DirectX::XMFLOAT4 & positionOfCenter,
			const IconicStyleGeneric & style
		);

		Button(
			ForceConstForecolorType force,
			const std::string& pageNo,
			const DirectX::XMFLOAT4& positionOfCenter,
			const IconicStyleGeneric& style
		);

	protected:
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;

		Renderer::CB::ForecolorInstance m_forecolor;

	private:
		bool OnRegion() const noexcept final;

		const MeshStyle::FnOnRegion c_fnOnRegion;
	};

	/// <summary>
	/// Abstract base class shared by checkbox and radio button controls.
	/// </summary>
	interface TwoStateButton abstract : Button
	{
		TwoStateButton(
			const std::string & pageNo,
			const DirectX::XMFLOAT2 & positionOfCenter,
			const IconicStyleGeneric & style
		);

	protected:
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) final;
		virtual bool IsSelected() const noexcept = 0;
		size_t GetCurrentTextureID() const noexcept final;

	private:
		const DirectX::XMFLOAT3 c_rgbBackup;
	};

	struct Checkbox : TwoStateButton
	{
		Checkbox(
			const std::string& pageNo,
			const DirectX::XMFLOAT2& positionOfCenter,
			const IconicStyleGeneric& style,
			bool& isSelected
		);

	protected:
		void OnClick() override;

	private:
		bool IsSelected() const noexcept final;

		bool& m_isSelected;
	};

	struct DialogGeneric;
	/// <summary>
	/// Controller class responsible for coordinating a set of radio buttons.
	/// </summary>
	struct ManagerRadioButton
	{
		struct RadioButton : TwoStateButton
		{
			friend ManagerRadioButton;

			using OptionalI18NString = std::optional<const I18N::I18NString>;
			RadioButton(
				const std::string& pageNo,
				const uint32_t pos,
				const DirectX::XMFLOAT2& positionOfCenter,
				ManagerRadioButton& manager,
				const IconicStyleGeneric& style,
				const OptionalI18NString label = std::nullopt
			);

		private:
			bool IsSelected() const noexcept final;

		protected:
			void OnClick() override;

			const uint32_t c_pos;

		private:
			ManagerRadioButton& m_manager;
		};

		ManagerRadioButton(
			DialogGeneric& parent,
			uint32_t& radioSelected
		) noexcept;
		void SetSelected(
			const RadioButton* radioButton
		) noexcept;
		void LabelRadioButton(
			RadioButton& radioButton,
			const I18N::I18NString& title
		);
		void NoLabelRadioButton(RadioButton& radioButton);
		RadioButton* GetSelected() noexcept;

	private:
		DialogGeneric& m_parent;
		uint32_t& m_radioSelected;
		Kernel::VectorNoCtor<RadioButton*> m_radioStates;
	};

	interface ButtonOneToolbarGeneric abstract : Button
	{
	protected:
		ButtonOneToolbarGeneric(
			const std::string & pageNo,
			const DirectX::XMFLOAT2 & positionOfCenter,
			const IconicStyleGeneric & style
		);
	};
}
