// ##########################################################################
// ### Copyright © 2015-2023 by W³odzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "GUIMath.h"
// Monad
#include "Core/DXSample.h"
#include "Kernel/Format.h"

using namespace DirectX;

namespace Monad
{
	using namespace Kernel;

	namespace GUI
	{
		namespace
		{
			constexpr DirectX::XMFLOAT2 GetNormalizedAspectRatio(
				float width,
				float height
			) noexcept
			{
				assert(0 != width && 0 != height);
				if (width >= height)
					return { 1.0f, height / width };
				else
					return { width / height, 1.0f };
			}
		}

		void GUIMath::ProcessAspectRatio(
			float dx,
			float dy) noexcept
		{
			const auto host = GetAspectRatio(
				GetNormalizedAspectRatio(dx, dy)
			);
			assert(host);
			XMFLOAT3 input(INV_LANDSCAPE_RATIO, 0.0f, 1.0f);
			XMStoreFloat3(&input,
				XMVector3Transform(
					XMLoadFloat3(
						&input
					),
					XMLoadFloat4x4(
						&g_dxSample->m_matrices.m_matrixProjection
					)
				)
			);
			m_scale = (2.0f * INV_LANDSCAPE_RATIO) / input.x;
			const float superFactor = INV_LANDSCAPE_RATIO / host;
			if (superFactor < 1.0f)
				m_scale *= superFactor;

			if (host < INV_LANDSCAPE_RATIO)
				m_cursorFactor = { 1.0f, 1.0f / host };
			else
				m_cursorFactor = { host * LANDSCAPE_RATIO, LANDSCAPE_RATIO };
		}
	}
}