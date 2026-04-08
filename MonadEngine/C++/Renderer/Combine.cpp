// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Combine.h"
// STD
#include <ranges>

using namespace DirectX;

namespace Monad::Renderer
{
	void Combine::OnFrameRender(
		const XMFLOAT4X4& modelSpaceMatrix
	)
	{
		for (const auto monad : m_renderedObjects)
			monad->OnFrameRenderInternal(modelSpaceMatrix);
	}

	void Combine::AddRenderedObject(
		First* object
	)
	{
		assert(nullptr != object);
		assert(!contains(object));
		m_renderedObjects.emplace_back(object);
	}

	void Combine::AddRenderedObjects(
		InitializerListMonads objects
	)
	{
		assert(objects.size() > 1);
#if defined _DEBUG
		// Similar to Release, but performs additional validation (assertions).
		for (const auto& object : objects)
			AddRenderedObject(object);
#else
		m_renderedObjects.append_range(objects);
#endif
	}

	bool Combine::contains(
		const First* object
	) const noexcept
	{
		assert(nullptr != object);
		return std::ranges::find(m_renderedObjects, object) != m_renderedObjects.cend();
	}
}
