// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "MeshCore.h"

namespace Monad::Renderer::InputLayout
{
	MeshBuilderArtPane::MeshBuilderArtPane(
		const DirectX::XMFLOAT4& paneRect,
		const double start,
		const double end
	)
	{
		const float startLerp = static_cast<float>(start),
			endLerp = static_cast<float>(end),
			x = Lerp(paneRect, startLerp),
			z = Lerp(paneRect, endLerp),
			f0 = 1.0f - endLerp,
			f1 = 1.0f - startLerp;
		AddQuad(
			{ { x, paneRect.y }, { f1, 1.0f } },
			{ { z, paneRect.y }, { f0, 1.0f } },
			{ { z, paneRect.w }, { f0, 0.0f } },
			{ { x, paneRect.w }, { f1, 0.0f } });
	}

	inline float MeshBuilderArtPane::Lerp(
		const DirectX::XMFLOAT4& paneRect,
		const float horizontalFactor
	) noexcept
	{
		return std::lerp(paneRect.x, paneRect.z, horizontalFactor);
	}
}