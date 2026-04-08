// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "First.h"

namespace Monad::Renderer
{
	void First::OnFrameRenderInternal(
		const DirectX::XMFLOAT4X4& modelSpaceMatrix
	)
	{
		if (IsVisible())
		{
			/// <summary>
			/// Simple implicit matrix stack implementatation
			/// </summary>
			/// <param name="modelSpaceMatrix">The new instance of the matrix on the top</param>
			auto pushedMatrix(modelSpaceMatrix);
			OnNewMatrix(pushedMatrix);
			OnFrameRender(pushedMatrix);
		}
	}

	bool First::IsVisible() const noexcept
	{
		return true;
	}

	void First::OnNewMatrix(
		[[maybe_unused]] DirectX::XMFLOAT4X4& matrixToAlter
	) const noexcept
	{}

	bool First::IsSystem() const noexcept
	{
		return false;
	}
}