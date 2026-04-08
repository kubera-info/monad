// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppColorIndex.h"
#include "Renderer/First.h"
#include "Renderer/Tex.h"

namespace Monad::App
{
	/// <summary>
	/// GPU-backed color palette manager.
	/// Generates and updates a 1D texture representing
	/// all available colors used for coloring.
	/// </summary>
	struct PaletteMap final : Renderer::First
	{
		/// <summary>
		/// Creates a palette texture with the given identifier.
		/// </summary>
		/// <param name="palette">Name of the palette resource.</param>
		PaletteMap(
			const std::string& palette
		);

		/// <summary>
		/// Assigns a new color to the given palette index.
		/// </summary>
		/// <param name="index">Index of the color entry.</param>
		void SetNewColor(
			size_t index
		) noexcept;

		/// <summary>
		/// Resets all palette colors back to their default values.
		/// Colors transition smoothly using animation.
		/// </summary>
		void ResetColors();

		/// <summary>
		/// Fluahes all palette colors.
		/// Stops colors transition smoothly using animation.
		/// </summary>
		void FlushColors();

		/// <summary>
		/// Container holding animated color entries.
		/// </summary>
		ArrayColors m_paletteContainer;

	private:
		/// <summary>
		/// Updates the GPU texture every frame.
		/// </summary>
		void OnFrameRender(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) final;

		/// <summary>
		/// Indicates that this resource is system-level
		/// and always active.
		/// </summary>
		bool IsSystem() const noexcept final;

		/// <summary>
		/// GPU texture storing palette colors.
		/// </summary>
		Renderer::Texture m_tex;

		/// <summary>
		/// Upload heap used to transfer palette data to the GPU.
		/// </summary>
		Renderer::PtrResource m_textureUploadHeap;
	};
}