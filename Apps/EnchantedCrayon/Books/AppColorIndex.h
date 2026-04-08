// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
// STD
#include <array>
// Monad
#include "Kernel/Del.h"
#include "Renderer/__MonadRenderer.h"
#include "Time/TimeLerp.h"

namespace Monad::App
{
	/// <summary>
	/// Maximum number of colors available in a single coloring palette.
	/// </summary>
	constexpr size_t ROWS_PALETTE_COUNT = 1024;

	/// <summary>
	/// Represents a static color holder.
	/// Stores a single RGB color without animation or interpolation.
	/// </summary>
	struct ColorIndexGeneric
	{
		ColorIndexGeneric() noexcept;
		ColorIndexGeneric(
			const DirectX::XMFLOAT3& newColor
		) noexcept;
		~ColorIndexGeneric() = default;

		OPER_DEL_NO_DEF_CTOR(ColorIndexGeneric);

		/// <summary>
		/// Sets the color directly.
		/// No interpolation or animation is applied.
		/// </summary>
		/// <param name="newColor">New RGB color value.</param>
		void AlterColor(
			const DirectX::XMFLOAT3& newColor
		) noexcept;

		/// <summary>
		/// Checks whether the color is pure white.
		/// White is treated as a special state meaning "uncolored".
		/// </summary>
		/// <returns>True if the color equals GUI white.</returns>
		bool IsWhite() const noexcept;

		/// <summary>
		/// Stored RGB color value.
		/// </summary>
		DirectX::XMFLOAT3 m_newColor;
	};

	/// <summary>
	/// Animated color holder.
	/// Extends ColorIndexGeneric with smooth interpolation
	/// between old and new color values.
	/// </summary>
	struct ColorIndex final : ColorIndexGeneric, private Time::TimeLerp
	{
		ColorIndex() noexcept;
		ColorIndex(
			const DirectX::XMFLOAT3& newColor
		) noexcept;
		~ColorIndex() = default;

		OPER_DEL_NO_DEF_CTOR(ColorIndex);

		/// <summary>
		/// Overrides the base method.
		/// Sets the color immediately without interpolation.
		/// This method is intentionally non-virtual for performance reasons.
		/// </summary>
		/// <param name="newColor">New color value.</param>
		void AlterColor(
			const DirectX::XMFLOAT3& newColor
		) noexcept;

		/// <summary>
		/// Sets a new target color and starts interpolation
		/// if the value differs from the current one.
		/// </summary>
		/// <param name="newColor">Target RGB color.</param>
		void SetNewColor(
			const DirectX::XMFLOAT3& newColor
		);

		/// <summary>
		/// Resets the color to the default GUI white.
		/// </summary>
		void ResetColor();

		/// <summary>
		/// Returns the current color value.
		/// The value may be interpolated depending on animation state.
		/// </summary>
		DirectX::XMFLOAT3 GetCurrentColor() const noexcept;

		void FlushColor();

	private:
		/// <summary>
		/// Previous color used as the start point for interpolation.
		/// </summary>
		DirectX::XMFLOAT3 m_oldColor;
	};

	/// <summary>
	/// Index zero is reserved as a meta-color.
	/// The first usable color index starts from this value.
	/// </summary>
	constexpr size_t FIRST_ENABLED = 1;

	/// <summary>
	/// Generic palette container for color indices.
	/// </summary>
	template<typename I>
	struct ArrayPalette final : std::array<I, ROWS_PALETTE_COUNT>
	{
		ArrayPalette() noexcept = default;
		~ArrayPalette() = default;

		ArrayPalette(const ArrayPalette&) = delete;
		ArrayPalette(ArrayPalette&&) = delete;
		ArrayPalette& operator=(ArrayPalette&&) = delete;

		/// <summary>
		/// Copies color values from another palette-like container.
		/// </summary>
		template<typename S>
		ArrayPalette& operator=(
			const S& source
			) noexcept
		{
			for (size_t index = FIRST_ENABLED; index < this->size(); ++index)
				this->operator[](index).AlterColor(
					source[index].m_newColor
				);

			return *this;
		}

		/// <summary>
		/// Move-assignment variant.
		/// </summary>
		template<typename S>
		ArrayPalette& operator=(
			S&& source
			) noexcept
		{
			for (size_t index = FIRST_ENABLED; index < this->size(); ++index)
				this->operator[](index).AlterColor(
					source[index].m_newColor
				);

			return *this;
		}
	};

	/// <summary>
	/// Static color palette (no animation).
	/// Can be stored in user profile or presets.
	/// </summary>
	using ArrayColorsGeneric = ArrayPalette<ColorIndexGeneric>;

	/// <summary>
	/// Animated color palette.
	/// Used for smooth transitions when user changes colors.
	/// </summary>
	using ArrayColors = ArrayPalette<ColorIndex>;
}