// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppMain.h"
#include "AppGesture.h"
#include "GUI/Background.h"
#include "Kernel/Events.h"
#include "Fonts/FontBuilder.h"

namespace Monad::App
{
	/// <summary>
	/// Main repository of all renderable objects.
	/// Acts as a root container for UI, rendering logic,
	/// palettes, dialogs, and runtime state.
	/// </summary>
	struct CombineWorld final : Renderer::Combine, Kernel::EventFrameMoveStub
	{
		CombineWorld();

		/// <summary>
		/// Returns current light position used by shaders.
		/// </summary>
		inline DirectX::XMFLOAT4 GetLight() const noexcept;

		/// <summary>
		/// Resets all colors in the active artwork.
		/// </summary>
		void ResetColors();

		/// <summary>
		/// Per-frame update callback.
		/// </summary>
		bool OnFrameMoveStub() noexcept final;

		/// <summary>
		/// HDR configuration constant buffer.
		/// </summary>
		const Renderer::HDRGeneric c_cBufferHDR;

		/// <summary>
		/// Morphing matrix used for animated transitions.
		/// </summary>
		Renderer::MorphingMatrix m_cBufferMorphing;

		/// <summary>
		/// Light position constant buffer.
		/// </summary>
		const Renderer::LightPosGeneric c_cBufferLightPos;

		/// <summary>
		/// Foreground color animation buffer.
		/// </summary>
		Fonts::ForecolorGeneric m_cBufferForecolor;

		/// <summary>
		/// Page transition animation controller.
		/// </summary>
		Pages::TimeLerpMorphingDecay m_pageDecay;

	private:
		/// <summary>
		/// Applies world-space transformation matrix.
		/// </summary>
		void OnNewMatrix(
			DirectX::XMFLOAT4X4& matrixToAlter
		) const noexcept final;

		/// <summary>
		/// Determines if the world should currently be rendered.
		/// </summary>
		bool IsVisible() const noexcept final;

	public:
		/// <summary>
		/// Aggregates all application subsystems and dialogs.
		/// Acts as the main runtime container.
		/// </summary>
		struct CombineFullRequirements final : Renderer::Combine
		{
			CombineFullRequirements();

		private:
			/// <summary>
			/// Self-registration helper.
			/// </summary>
			const Kernel::Me<CombineFullRequirements> c_me;

		public:
			std::unique_ptr<DialogReportGeneric> m_dlg;
		};

		/// <summary>
		/// Root container of all runtime systems.
		/// </summary>
		std::unique_ptr<CombineFullRequirements> m_full;

		/// <summary>
		/// Creates and initializes the full application state.
		/// </summary>
		void CreateFull();
	};

	/// <summary>
	/// Global access pointer to the active CombineFullRequirements instance.
	/// </summary>
	extern CombineWorld::CombineFullRequirements* g_combineFullRequirements;
}