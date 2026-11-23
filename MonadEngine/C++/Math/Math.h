// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>

// STD
#include <optional>
#include <vector>

namespace Monad::Math
{
	/// <summary>
	/// Represents an ellipse or arc discretized into line segments.
	/// </summary>
	struct Ellipse final
	{
		/// <summary>
		/// Constructs an ellipse or arc.
		/// </summary>
		/// <param name="sidesCount">Number of segments used to approximate the ellipse.</param>
		/// <param name="radiusX">Radius along the X axis.</param>
		/// <param name="radiusY">
		/// Optional radius along the Y axis.  
		/// If not provided, <paramref name="radiusX"/> is used.
		/// </param>
		/// <param name="angle">Arc angle in radians.</param>
		/// <param name="start">Starting angle in radians.</param>
		Ellipse(
			const uint32_t sidesCount,
			const float radiusX = 1.0f,
			const std::optional<const float> radiusY = std::nullopt,
			const float angle = DirectX::XM_2PI,
			const float start = 0.0f
		);

		/// <summary>
		/// Returns the vertex at the given index.
		/// </summary>
		const DirectX::XMFLOAT2& operator[](
			size_t n
			) const noexcept;

		/// <summary>
		/// Returns the number of segments used by the ellipse.
		/// </summary>
		const size_t GetSidesCount() const noexcept;

		/// <summary>
		/// Horizontal and vertical radii.
		/// </summary>
		const float c_radiusX, c_radiusY;

	private:
		/// <summary>
		/// Computes a single point on the ellipse for the given phase.
		/// </summary>
		/// <param name="phase">Current angle in radians.</param>
		void operator+=(
			const float phase
			);

		/// <summary>
		/// Precomputed vertex list.
		/// </summary>
		std::vector<DirectX::XMFLOAT2> m_points;
	};
}
