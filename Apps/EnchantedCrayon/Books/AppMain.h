// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppArtPane.h"
#include "AppPalette.h"
#include "AppToolbox.h"

namespace Monad::App
{
	/// <summary>
	/// Main application view.
	/// Contains all UI elements required for coloring:
	/// palette, toolbox and drawing surface.
	/// </summary>
	struct CombineMain final : Renderer::Combine
	{
		CombineMain();
		~CombineMain() = default;

	private:
		/// <summary>
		/// Color palette dialog used to select drawing colors.
		/// </summary>
		DialogPalette m_paletteDialog;

	public:
		/// <summary>
		/// Toolbox dialog containing additional tools
		/// (e.g. print, settings, help).
		/// </summary>
		DialogToolbox m_toolboxDialog;

		/// <summary>
		/// Main drawing area containing the artwork to be colored.
		/// </summary>
		struct DialogPicture final : GUI::DialogGeneric
		{
			DialogPicture();

			/// <summary>
			/// Sets the currently active artwork.
			/// </summary>
			/// <param name="art">Iterator pointing to the selected artwork.</param>
			void SetArt(
				const Repository::ConstArtIterator art
			) noexcept;

			using CombineArtTex2Pane = CombineArtPreview<ShaderTex2Pane>;

			/// <summary>
			/// Interactive pane displaying the coloring image.
			/// </summary>
			CombineArtTex2Pane m_panesDialog;
		} m_pictureDialog;
	};
}