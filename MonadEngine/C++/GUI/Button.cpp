// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Button.h"

// Monad
#include "Fonts/__MonadFont.h"
#include "Core/Win32ApplicationGeneric.h"
#include "Kernel/Format.h"
#include "Dialog.h"
#include "Repository/Repo.h"

using namespace DirectX;

namespace Monad
{
	using namespace Fonts;
	using namespace Kernel;
	using namespace Renderer;
	using namespace Repositories;

	namespace GUI
	{
		ForceConstForecolorType ForceConstForecolor;
		Button::Button(
			const std::string& pageNo,
			const XMFLOAT4& positionOfCenter,
			const IconicStyleGeneric& style
		) :
			Pane{ positionOfCenter, style.c_style.c_technique, style.c_icons, style.c_style.c_meshFilename },
			Clickable{ pageNo, style.c_clicksSounds },
			m_forecolor{ "forecolor"_constantBuffer },
			c_fnOnRegion{ style.c_style.c_onRegionFun }
		{
			m_forecolor.GetBuffer() = { style.c_rgb.GetFloatSRGB() };
		}

		Button::Button(
			ForceConstForecolorType,
			const std::string& pageNo,
			const XMFLOAT4& positionOfCenter,
			const IconicStyleGeneric& style
		) :
			Pane{ positionOfCenter, style.c_style.c_technique, style.c_icons, style.c_style.c_meshFilename },
			Clickable{ pageNo, style.c_clicksSounds },
			m_forecolor{ "forecolor"_constantBuffer, style.c_rgb.GetFloatSRGB() },
			c_fnOnRegion{ style.c_style.c_onRegionFun }
		{}

		bool Button::OnRegion() const noexcept
		{
			return c_fnOnRegion(m_positionOfCenter) && IsVisible() && !Monad::Pages::g_MorphingDecay->IsWorking();
		}

		void Button::OnComputePerWorldFrame(const DirectX::XMFLOAT4X4& modelSpaceMatrix)
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			m_forecolor.SetMe(GetCurrentConfig());
		}

		TwoStateButton::TwoStateButton(
			const std::string& pageNo,
			const XMFLOAT2& positionOfCenter,
			const IconicStyleGeneric& style
		) :
			Button{ pageNo, { positionOfCenter.x, positionOfCenter.y, ICON_RADIUS, ICON_RADIUS }, style },
			c_rgbBackup{ style.c_rgb.GetFloatSRGB() }
		{}

		void TwoStateButton::OnComputePerWorldFrame(
			const XMFLOAT4X4& modelSpaceMatrix
		)
		{
			if (IsSelected())
				m_forecolor.GetBuffer() = c_rgbBackup;
			else
			{
				constexpr float deselectedGrey = 0.075f;
				m_forecolor.GetBuffer() = { deselectedGrey, deselectedGrey, deselectedGrey };
			}

			__super::OnComputePerWorldFrame(modelSpaceMatrix);
		}

		size_t TwoStateButton::GetCurrentTextureID() const noexcept
		{
			assert(!m_textures.empty());
			return IsSelected() ? 0 : m_textures.size() - 1;
		}

		Checkbox::Checkbox(
			const std::string& pageNo,
			const XMFLOAT2& positionOfCenter,
			const IconicStyleGeneric& style,
			bool& isSelected
		) :
			TwoStateButton{
			pageNo,
			positionOfCenter,
			style },
			m_isSelected{ isSelected }
		{}

		bool Checkbox::IsSelected() const noexcept
		{
			return m_isSelected;
		}

		void Checkbox::OnClick()
		{
			m_isSelected = !m_isSelected;
		}

		ManagerRadioButton::RadioButton::RadioButton(
			const std::string& pageNo,
			const uint32_t pos,
			const XMFLOAT2& positionOfCenter,
			ManagerRadioButton& manager,
			const IconicStyleGeneric& style,
			const OptionalI18NString label
		) :
			TwoStateButton{
				pageNo,
				positionOfCenter,
				style },
				c_pos{ pos },
				m_manager{ manager }
		{
			if (label)
				m_manager.LabelRadioButton(
					*this,
					*label
				);
			else
				m_manager.NoLabelRadioButton(*this);
		}

		bool ManagerRadioButton::RadioButton::IsSelected() const noexcept
		{
			return m_manager.GetSelected() == this;
		}

		void ManagerRadioButton::RadioButton::OnClick()
		{
			m_manager.SetSelected(this);
		}

		ManagerRadioButton::ManagerRadioButton(
			DialogGeneric& parent,
			uint32_t& radioSelected
		) noexcept :
			m_parent{ parent },
			m_radioSelected{ radioSelected }
		{}

		void ManagerRadioButton::SetSelected(
			const RadioButton* radioButton
		) noexcept
		{
			auto radio = std::ranges::find_if(
				m_radioStates,
				[=](const RadioButton* radioToCheck) noexcept
				{
					return radioToCheck == radioButton;
				});
			if (radio != m_radioStates.cend())
			{
				m_radioSelected = radioButton->c_pos;
				return;
			}
			assert(false);
		}

		ManagerRadioButton::RadioButton* ManagerRadioButton::GetSelected() noexcept
		{
			auto radio = std::ranges::find_if(
				m_radioStates,
				[=](const RadioButton* radioToCheck) noexcept
				{
					return radioToCheck->c_pos == m_radioSelected;
				});
			if (m_radioStates.cend() != radio)
				return *radio;
			return nullptr;
		}

		void ManagerRadioButton::LabelRadioButton(
			RadioButton& radioButton,
			const I18N::I18NString& title
		)
		{
			m_parent.LabelRadioButton(
				radioButton,
				title
			);
			m_radioStates.push_back(&radioButton);
		}

		void ManagerRadioButton::NoLabelRadioButton(
			RadioButton& radioButton
		)
		{
			m_parent.AddRenderedObject(&radioButton);
			m_radioStates.push_back(&radioButton);
		}

		ButtonOneToolbarGeneric::ButtonOneToolbarGeneric(
			const std::string& pageNo,
			const XMFLOAT2& positionOfCenter,
			const IconicStyleGeneric& style
		) :
			Button{
				ForceConstForecolor,
				pageNo,
				{ positionOfCenter.x, positionOfCenter.y, ICON_RADIUS, ICON_RADIUS },
				style }
		{}
	}
}
