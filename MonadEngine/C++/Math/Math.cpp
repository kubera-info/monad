// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Math.h"

using namespace DirectX;

namespace Monad::Math
{
	Ellipse::Ellipse(
		const uint32_t sidesCount,
		const float radiusX,
		const std::optional<const float> radiusY,
		const float angle,
		const float start
	) :
		c_radiusX(radiusX
		),
		c_radiusY(
			radiusY.has_value()
			? *radiusY
			: radiusX
		)
	{
		m_points.reserve(sidesCount + 1); // +1 for the last point
		float phase = start;
		const float step = angle / sidesCount;
		for (uint32_t ind = 0; ind < sidesCount; ++ind)
		{
			*this += phase;
			phase += step;
		}
		if (XM_2PI == angle && 0.0f == start && !m_points.empty())
			m_points.push_back(operator[](0));
		else
			*this += phase;
	}

	const XMFLOAT2& Ellipse::operator[](
		size_t n
		) const noexcept
	{
#if defined _DEBUG
		return m_points.at(n);
#else
		return m_points[n];
#endif
	}

	const size_t Ellipse::GetSidesCount() const noexcept
	{
		return static_cast<uint32_t>(m_points.size()) - 1;
	}

	void Ellipse::operator+=(
		const float phase
		)
	{
		float sinVal, cosVal;
		XMScalarSinCosEst(
			&sinVal,
			&cosVal,
			phase
		);
		m_points.emplace_back(
			cosVal * c_radiusX,
			-sinVal * c_radiusY
		);
	}
}