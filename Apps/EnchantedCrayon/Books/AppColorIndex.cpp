// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppColorIndex.h"
// Monad
#include "GUI/__MonadGUI.h"
#include "Kernel/Templates.h"

using namespace DirectX;
using namespace std;

namespace Monad
{
	using namespace GUI;
	using namespace Kernel;
	using namespace Renderer;

	namespace App
	{
#pragma region ColorIndexGeneric

		// ---------------------------------------------------------------------
		// ColorIndexGeneric
		// ---------------------------------------------------------------------
		// Base class representing a simple color holder.
		// Does not perform interpolation or animation.
		// ---------------------------------------------------------------------

		// Default constructor.
		// Initializes the color with the default GUI color (white).
		ColorIndexGeneric::ColorIndexGeneric() noexcept :
			ColorIndexGeneric{ GUI_WHITE }
		{}

		// Constructor initializing the color with a specific value.
		ColorIndexGeneric::ColorIndexGeneric(const XMFLOAT3& newColor) noexcept :
			m_newColor{ newColor }
		{}

		// Directly sets a new color value.
		// No interpolation or animation is performed.
		void ColorIndexGeneric::AlterColor(const DirectX::XMFLOAT3& newColor) noexcept
		{
			m_newColor = newColor;
		}

		// Checks whether the current color is equal to the default GUI white.
		bool ColorIndexGeneric::IsWhite() const noexcept
		{
			return strong_ordering::equivalent == GUI_WHITE <=> m_newColor;
		}

#pragma endregion

#pragma region ColorIndex

		// ---------------------------------------------------------------------
		// ColorIndex
		// ---------------------------------------------------------------------
		// Extended color handler supporting smooth interpolation
		// between color values using TimeLerp.
		// ---------------------------------------------------------------------

		// Default constructor.
		// Initializes the color using the default GUI color.
		ColorIndex::ColorIndex() noexcept :
			ColorIndex{ GUI_WHITE }
		{}

		// Constructor initializing color and interpolation parameters.
		ColorIndex::ColorIndex(
			const XMFLOAT3& newColor
		) noexcept :
			ColorIndexGeneric{ newColor },
			TimeLerp{
				0.75,   // interpolation duration
				0.0,    // start value
				1.0     // end value
			},
			m_oldColor{ newColor }
		{}

		// Immediately changes the color without interpolation.
		// Also updates the stored previous color.
		void ColorIndex::AlterColor(
			const XMFLOAT3& newColor
		) noexcept
		{
			__super::AlterColor(newColor);
			m_oldColor = newColor;
		}

		// Sets a new target color and starts interpolation
		// if the new color differs from the current one.
		void ColorIndex::SetNewColor(
			const XMFLOAT3& newColor
		)
		{
			m_oldColor = GetCurrentColor();

			if (strong_ordering::equivalent != m_oldColor <=> newColor ||
				strong_ordering::equivalent != m_oldColor <=> m_newColor)
			{
				m_newColor = newColor;
				Start();
			}
		}

		// Resets the color back to the default GUI white.
		void ColorIndex::ResetColor()
		{
			SetNewColor(GUI_WHITE);
		}

		// Returns the currently interpolated color value.
		XMFLOAT3 ColorIndex::GetCurrentColor() const noexcept
		{
			XMFLOAT3 ret;
			XMStoreFloat3(
				&ret,
				XMVectorLerp(
					XMLoadFloat3(&m_oldColor),
					XMLoadFloat3(&m_newColor),
					GetCurrentValue()
				)
			);
			return ret;
		}

		void ColorIndex::FlushColor()
		{
			Flush();
		}
#pragma endregion
	}
}