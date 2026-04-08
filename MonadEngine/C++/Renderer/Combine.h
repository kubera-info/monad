// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/BytesVector.h"
#include "First.h"

namespace Monad::Renderer
{
	/// <summary>
	/// Container that aggregates multiple renderable monads
	/// and renders them as a single unit.
	/// </summary>
	struct Combine : First
	{
		/// <summary>
		/// Initializer list of monad pointers.
		/// </summary>
		using InitializerListMonads = std::initializer_list<First*>;

		/// <summary>
		/// Renders all contained objects using the given model-space transform.
		/// </summary>
		void OnFrameRender(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;

		/// <summary>
		/// Adds a single renderable object to the container.
		/// </summary>
		void AddRenderedObject(
			First* object
		);

		/// <summary>
		/// Adds multiple renderable objects to the container.
		/// </summary>
		void AddRenderedObjects(
			InitializerListMonads objects
		);

		/// <summary>
		/// Checks whether the specified object is already contained.
		/// </summary>
		bool contains(
			const First* object
		) const noexcept;

	private:
		/// <summary>
		/// Collection of renderable objects managed by this container.
		/// </summary>
		Kernel::VectorNoCtor<First*> m_renderedObjects;
	};
}
