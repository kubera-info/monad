// ##########################################################################
// ### Copyright © 2015-2023 by W³odzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Background.h"

// Monad
#include "Core/DXSample.h"
#include "Renderer/__MonadRenderer.h"

using namespace DirectX;

namespace Monad::GUI
{
	using namespace Renderer;

	namespace
	{
		XMFLOAT4X4 GetTransform() noexcept
		{
			XMFLOAT4X4 backgroundTransform;
			XMStoreFloat4x4(&backgroundTransform,
				XMMatrixMultiply(
					XMMatrixTranslation(0.0f, 0.0f, -1.0f),
					XMMatrixScaling(
						-g_dxSample->m_matrices.m_monitorProps.m_cursorFactor.x,
						g_dxSample->m_matrices.m_monitorProps.m_cursorFactor.y,
						1.0f)));
			return backgroundTransform;
		}
	}

	Background::Background() :
		CalculatePosition{
			GetTransform(),
			"wallpaper"_technique,
			InitializerListTextures{ "wallpaper"_shaderResource },
			T2_ART,
			"pane"_mesh }
	{}
}