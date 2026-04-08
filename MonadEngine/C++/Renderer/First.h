// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
// Monad
#include "Kernel/Del.h"

namespace Monad::Renderer
{
	/// <summary>
	/// Base class for all renderable monads.
	///
	/// A monad represents a renderable object that may form a hierarchy
	/// (parent–child relationships). Monads cannot be copied or moved;
	/// they must be handled through pointers.
	/// </summary>
	interface First abstract
	{
		/// <summary>
		/// Entry point for rendering this monad.
		/// Called internally by the rendering pipeline.
		/// </summary>
		/// <param name="modelSpaceMatrix">
		/// Current transformation matrix inherited from parent monads.
		/// </param>
		void OnFrameRenderInternal(
			const DirectX::XMFLOAT4X4 & modelSpaceMatrix
		);

		/// <summary>
		/// Determines whether this monad should be rendered.
		/// </summary>
		virtual bool IsVisible() const noexcept;

	protected:
		First() noexcept = default;
		OPER_DEF_NO_DEF_CTOR(First);
		virtual ~First() = default;

		/// <summary>
		/// Allows modification of the transformation matrix
		/// before rendering.
		/// </summary>
		/// <param name="matrixToAlter">
		/// Matrix to be modified for this monad.
		/// </param>
	virtual void OnNewMatrix(
			DirectX::XMFLOAT4X4& matrixToAlter
		) const noexcept;
	/// <summary>
	/// Performs actual rendering of the monad.
	/// </summary>
	/// <param name="modelSpaceMatrix">
	/// Final transformation matrix for rendering.
	/// </param>

	virtual void OnFrameRender(
		const DirectX::XMFLOAT4X4& modelSpaceMatrix
	) = 0;
	// virtual bool IsTemporary() const noexcept;
	virtual bool IsSystem() const noexcept;
	};

	/// <summary>
	/// Helper class that applies a static transformation
	/// before rendering.
	/// </summary>
	template<typename R>
	struct CalculatePosition : R
	{
	protected:
		template<typename... A>
		CalculatePosition(
			const DirectX::XMFLOAT4X4& staticSpace,
			A&&... args
		) :
			R(args...),
			m_staticSpace{ staticSpace }
		{}

		/// <summary>
				/// Multiplies the current transformation matrix
				/// by a predefined static matrix.
				/// </summary>
		void OnNewMatrix(
			DirectX::XMFLOAT4X4& matrixToAlter
		) const noexcept override
		{
			DirectX::XMStoreFloat4x4(
				&matrixToAlter,
				DirectX::XMMatrixMultiply(
					DirectX::XMLoadFloat4x4(&m_staticSpace),
					DirectX::XMLoadFloat4x4(&matrixToAlter)
				)
			);
		}

	private:
		const DirectX::XMFLOAT4X4 m_staticSpace;
	};
}