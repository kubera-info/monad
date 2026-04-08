// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Platform
#include <d3d12.h>
// STD
#include <string>
#include <vector>
// Monad
#include "Renderer/__MonadRenderer.h"
#include "Kernel/Templates.h"

namespace Monad::Renderer::InputLayout
{
	/// <summary>
	/// Wrapper around D3D12_INPUT_ELEMENT_DESC providing a simpler constructor.
	/// </summary>
	struct INPUT_ELEMENT_DESC : D3D12_INPUT_ELEMENT_DESC
	{
		INPUT_ELEMENT_DESC(
			const char* semanticName,
			SEMANTIC_INDEX semanticIndex,
			DXGI_FORMAT format
		) noexcept;
	};

	/// <summary>
	/// Container for input layout elements.
	/// </summary>
	struct VectorInputElementsDesc : std::vector<INPUT_ELEMENT_DESC>
	{
		using std::vector<INPUT_ELEMENT_DESC>::vector;

		/// <summary>
		/// Builds a D3D12 input layout descriptor.
		/// </summary>
		D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() const noexcept;
	};

	/// <summary>
	/// Collection of named input layouts.
	/// </summary>
	struct MapInputLayouts : private
		Kernel::UnorderedMapString<VectorInputElementsDesc>
	{
		using Kernel::UnorderedMapString<VectorInputElementsDesc>::UnorderedMapString;

		/// <summary>
		/// Returns an input layout descriptor by name.
		/// </summary>
		D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc(
			const std::string& id
		) const;

		/// <summary>
		/// Merges another layout map into this one.
		/// </summary>
		MapInputLayouts& operator+=(
			const MapInputLayouts& source
			);

		/// <summary>
		/// Returns a merged copy of two layout maps.
		/// </summary>
		MapInputLayouts operator+(
			const MapInputLayouts& source
			);
	};

	/// <summary>
	/// Standard input layouts for basic geometry.
	/// </summary>
	MapInputLayouts Basics();

	/// <summary>
	/// Input layouts used for font and emoji rendering.
	/// </summary>
	MapInputLayouts FontEndEmoji();

	/// <summary>
	/// Input layouts for textured image rendering.
	/// </summary>
	MapInputLayouts Image();
}
