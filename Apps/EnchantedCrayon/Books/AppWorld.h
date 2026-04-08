// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppCursor.h"
#include "AppGesture.h"
#include "AppMain.h"
#include "AppPaletteMap.h"
#include "AppSettings.h"
#include "AppSketch.h"
#include "AppStartMenu.h"
#include "GUI/Background.h"
#include "Kernel/Events.h"

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
		/// Applies a new color to the currently selected pixel.
		/// </summary>
		void SetNewColor(
			float x,
			float y
		) noexcept;

		/// <summary>
		/// Resets all colors in the active artwork.
		/// </summary>
		void ResetColors();

		void FlushColors();

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

		/// <summary>
		/// Background rendering layer.
		/// </summary>
		GUI::Background m_background;

		/// <summary>
		/// Cursor rendered as a torus shape.
		/// </summary>
		ShaderCursorTorus m_torusCursor;

		/// <summary>
		/// Splash screen shown during application startup.
		/// </summary>
		struct DialogSplash final : GUI::DialogFullScreenGeneric
		{
			DialogSplash();
		} m_splashDialog;

	public:
		/// <summary>
		/// Aggregates all application subsystems and dialogs.
		/// Acts as the main runtime container.
		/// </summary>
		struct CombineFullRequirements final : Renderer::Combine
		{
			CombineFullRequirements(
				uint32_t& langID
			);

			/// <summary>
			/// Sets the currently active artwork.
			/// </summary>
			void SetArt(
				Repository::ConstArtIterator art
			);

			/// <summary>
			/// Initializes sketch-related runtime resources.
			/// </summary>
			void CreateSketchVar() noexcept;

			/// <summary>
			/// Resets all color palettes.
			/// </summary>
			void ResetColors();

			void FlushColors();

			/// <summary>
			/// Synchronizes all palette maps and returns the active palette.
			/// </summary>
			const ArrayColors& SetPalette() noexcept;

			/// <summary>
			/// Applies a new color to all palette layers.
			/// </summary>
			void SetNewColor(
				size_t index
			) noexcept;

		private:
			/// <summary>
			/// Self-registration helper.
			/// </summary>
			const Kernel::Me<CombineFullRequirements> c_me;

		public:
			/// <summary>
			/// Cursor used for drawing and selection.
			/// </summary>
			CombineCursorCrayon m_arrowCursor;

			/// <summary>
			/// Main drawing page.
			/// </summary>
			CombineMain m_artPage;

			/// <summary>
			/// Color palettes used by the application.
			/// </summary>
			PaletteMap m_paletteMap, m_secondaryPaletteMap,
				m_paletteMap1, m_secondaryPaletteMap1;

			/// <summary>
			/// Art selection dialog.
			/// </summary>
			DialogArtSelection m_artSelectionDialog;

		private:
			/// <summary>
			/// Exit confirmation dialog.
			/// </summary>
			DialogExitApp m_exitAppDialog;

			/// <summary>
			/// Rendering and printing shader.
			/// </summary>
			ShaderSketch m_sketch;

			/// <summary>
			/// Printing progress dialog.
			/// </summary>
			struct DialogPrint final : GUI::DialogFullScreenGeneric
			{
				DialogPrint();
			} m_printDialog;

			DialogSettings m_settingsDialog;
			DialogAbout m_aboutDialog;
			DialogDesc m_descDialog;
			DialogOpenSource m_openSourceDialog;

			/// <summary>
			/// Swipe gesture handler.
			/// </summary>
			Swipe m_swipe;
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