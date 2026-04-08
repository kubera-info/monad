// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>

// Monad
#include "Kernel/Patterns.h"
#include "Renderer/HDR.h"
#include "Audio/Jingle.h"

namespace Monad::GUI
{
	struct MeshStyle final
	{
		/// <summary>
		/// Function signature used to test hit detection for mouse or other pointing devices.
		/// </summary>
		using FnOnRegion = bool(*) (const DirectX::XMFLOAT4& positionOfCenter);

		/// <summary>
		/// Evaluates whether the object is currently targeted by a pointing device.
		/// </summary>
		const FnOnRegion c_onRegionFun;
		const std::string
			c_meshFilename,
			c_technique;
	};

	struct IconicStyleGeneric
	{
		IconicStyleGeneric(
			const MeshStyle& style,
			const DirectX::XMFLOAT3& rgb,
			const Audio::RandomJingle& clicksSounds,
			const Kernel::VectorString& icons
		);

		const MeshStyle& c_style;
		const Renderer::HDR::PowerFloat4 c_rgb;
		/// <summary>
		/// Manages randomized jingle playback for non-deterministic audio behavior.
		/// </summary>
		const Audio::RandomJingle c_clicksSounds;
		const Kernel::VectorString c_icons;
	};

	/// <summary>
	/// Icon-based GUI style definition.
	/// Wraps visual appearance, interaction behavior,
	/// and optional sound feedback for UI elements.
	/// </summary>
	struct IconicStyle : GUI::IconicStyleGeneric
	{
		/// <summary>
		/// Creates an icon style using a custom color, sound set and icon list.
		/// </summary>
		IconicStyle(
			const DirectX::XMFLOAT3& rgb,
			const Audio::RandomJingle& clicksSounds,
			const Kernel::VectorString& icons
		);

		/// <summary>
		/// Creates an icon style using a custom mesh style.
		/// </summary>
		IconicStyle(
			const GUI::MeshStyle& style,
			const Audio::RandomJingle& clicksSounds,
			const Kernel::VectorString& icons
		);

		/// <summary>
		/// Creates an icon style with a given color and default sound.
		/// </summary>
		IconicStyle(
			const DirectX::XMFLOAT3& rgb,
			const Kernel::VectorString& icons
		);

		/// <summary>
		/// Creates an icon style using default color and no sound.
		/// </summary>
		IconicStyle(
			const Kernel::VectorString& icons
		);

		/// <summary>
		/// Creates an icon style for a language or flag icon.
		/// </summary>
		IconicStyle(
			const std::string& flag
		);
	};

	/// <summary>
	/// Generic rectangular button style using a single color.
	/// </summary>
	extern const GUI::MeshStyle ONE_COLOR_STYLE;
}
