// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Templates.h"
#include "Printer/Sketch.h"
#include "Renderer/Shader.h"
#include "Renderer/Targets.h"

namespace Monad::App
{
	/// <summary>
	/// Shader responsible for rendering the artwork into an off-screen
	/// texture used for printing or exporting.
	/// </summary>
	struct ShaderSketch final : Print::ShaderSketchGeneric
	{
		ShaderSketch();

		/// <summary>
		/// Starts the printing process for the currently selected artwork.
		/// This method prepares the render target and schedules the print task.
		/// </summary>
		void OnPrint();

		/// <summary>
		/// Releases the render target used for printing.
		/// Called after the print operation has completed.
		/// </summary>
		void ScreenshotRT() noexcept final;

	private:
		/// <summary>
		/// Per-frame rendering callback used to render the artwork
		/// into an off-screen texture.
		/// </summary>
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) final;

		/// <summary>
		/// Unique identifier of the render target used for printing.
		/// </summary>
		const uint32_t m_renderTargetID;

		/// <summary>
		/// Optional GPU texture handle assigned during rendering.
		/// </summary>
		Renderer::OptionalUInt m_texID;
	};
}