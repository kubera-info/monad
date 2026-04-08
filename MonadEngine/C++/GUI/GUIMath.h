// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
// STD
#include <algorithm>
#include <limits>

struct DXSample;

namespace Monad::GUI
{
	/// <summary>
	/// Handles aspect ratio correction and scaling between the application’s fixed 16:9 layout 
	/// and the runtime monitor resolution.”
	/// </summary>
	struct GUIMath
	{
		void ProcessAspectRatio(
			const float dx,
			const float dy
		) noexcept;

		DirectX::XMFLOAT2 m_cursorFactor;
		float m_scale;
	};

	template<typename F>
		requires std::is_floating_point_v<F>
	constexpr F GetNormalizedAspectRatio2(
		const F width,
		const F height
	) noexcept
	{
		assert(width && height);
		__assume(width && height);
		return std::min(height / width, width / height);
	}

	constexpr float GetAspectRatio(
		const DirectX::XMFLOAT2& vector
	) noexcept
	{
		assert(vector.y);
		__assume(0 < vector.y);
		return vector.x / vector.y;
	}

	constexpr float GetRight(
		const DirectX::XMFLOAT4& vector
	) noexcept
	{
		return vector.x + vector.z;
	}

	constexpr float GetBottom(
		const DirectX::XMFLOAT4& vector
	) noexcept
	{
		return vector.y + vector.w;
	}
}
