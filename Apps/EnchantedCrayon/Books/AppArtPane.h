// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppRepo.h"
#include "GUI/GUI.h"
#include "Renderer/Twin.h"

namespace Monad::App
{
	/// <summary>
	/// Base class for all art-rendering shaders.
	/// Handles palette binding and textured quad rendering.
	/// </summary>
	interface ShaderArtGeneric abstract : Renderer::ShaderTextured<Renderer::ShaderNoLight>
	{
		ShaderArtGeneric(
			ArrayColors & colors,
			const std::string & palette,
			const DirectX::XMFLOAT4 & paneRect
		);

		/// <summary>
		/// Selects the currently displayed artwork.
		/// Updates both texture and palette binding.
		/// </summary>
		/// <param name="art">Iterator pointing to the artwork entry</param>
		void SetArt(
			const Repository::ConstArtIterator art
		) noexcept;

	private:
		/// <summary>
		/// Updates shader constants each frame.
		/// Used internally by the rendering pipeline.
		/// </summary>
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) final;

		/// <summary>
		/// Name of the palette resource bound to this shader.
		/// Each palette has a dedicated shader slot.
		/// </summary>
		const std::string m_paletteName;

		/// <summary>
		/// Reference to the color array used by the shader.
		/// </summary>
		ArrayColors& m_colors;

	protected:
		/// <summary>
		/// Rectangle defining the screen-space area of the pane.
		/// </summary>
		const DirectX::XMFLOAT4 c_paneRect;
	};

	/// <summary>
	/// Preview version of art shader.
	/// Used for displaying thumbnails or intermediate previews.
	/// </summary>
	struct ShaderArtPreview : ShaderArtGeneric
	{
		ShaderArtPreview(
			ArrayColors& colors,
			const std::string& palette
		);

		/// <summary>
		/// Defines visible horizontal range of the preview.
		/// Used for animated transitions between artworks.
		/// </summary>
		/// <param name="start">Left boundary (0..1)</param>
		/// <param name="end">Right boundary (0..1)</param>
		void SetHorizontalRange(
			const double start,
			const double end
		);

	private:
		/// <summary>
		/// Current horizontal clipping range.
		/// Used during animated transitions.
		/// </summary>
		double m_start = 0.0, m_end = 1.0;
	};

	/// <summary>
	/// Secondary preview layer.
	/// Visibility depends on application state.
	/// </summary>
	struct ShaderArtPreviewSecond : ShaderArtPreview
	{
		using ShaderArtPreview::ShaderArtPreview;

	protected:
		/// <summary>
		/// Determines whether this preview should be rendered.
		/// </summary>
		bool IsVisible() const noexcept override;
	};

	/// <summary>
	/// Interactive texture pane used for color picking.
	/// Supports mouse interaction.
	/// </summary>
	struct ShaderTex2Pane : ShaderArtGeneric, private GUI::Clickable, Kernel::EventPointerUpStub
	{
		ShaderTex2Pane(
			ArrayColors& colors,
			const std::string& palette,
			const DirectX::XMFLOAT4& dialogFrame
		);

	private:
		/// <summary>
		/// Handles click action inside the color pane.
		/// Converts cursor position into color coordinates.
		/// </summary>
		void OnClick() noexcept final;

		/// <summary>
		/// Mouse release handler.
		/// Returns false if event was consumed.
		/// </summary>
		bool OnPointerUpStub() final;

		/// <summary>
		/// Checks whether cursor is inside the interactive region.
		/// </summary>
		bool OnRegion() const noexcept final;

		/// <summary>
		/// Precomputed scaling and offset values for coordinate mapping.
		/// </summary>
		const DirectX::XMFLOAT2 c_size, c_offset;
	};

	/// <summary>
	/// Utility wrapper combining two art preview instances.
	/// Used for animated transitions between artworks.
	/// </summary>
	template<typename A>
	struct CombineArtPreview : Renderer::CombineFrames<A>
	{
		using Renderer::CombineFrames<A>::CombineFrames;

		/// <summary>
		/// Assigns the same artwork to both frames.
		/// </summary>
		void SetArt(
			Repository::ConstArtIterator art
		) noexcept
		{
			this->m_frame->SetArt(art);
			this->m_frame1->SetArt(art);
		}

		/// <summary>
		/// Updates visible range of the active preview.
		/// </summary>
		void SetHorizontalRange(
			const double start,
			const double end
		)
		{
			if (this->m_frame->IsVisible())
				this->m_frame->SetHorizontalRange(start, end);
			else
				this->m_frame1->SetHorizontalRange(start, end);
		}
	};
}