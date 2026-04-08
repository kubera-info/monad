// ############################################################################
// ### Copyright © 2016–2024 by Włodzimierz O. Kubera. All rights reserved. ###
// ############################################################################
//
//-----------------------------------------------------------------------------
// File: D3DSaver.h
//
// Description:
//   Framework support for Direct3D-based Windows screen savers.
//   Provides runtime mode detection and initialization parameters.
//
//   Based on legacy DirectX screen saver model.
//
//-----------------------------------------------------------------------------

#pragma once

// Platform
#include <windows.h>

namespace Monad::Screensaver
{
	/// <summary>
	/// Supported execution modes of the screen saver.
	/// </summary>
	enum class SCREENSAVER_MODE
	{
		CONFIG,         ///< Configuration dialog
		PREVIEW,        ///< Preview window inside Display Properties
		FULL,           ///< Full-screen screensaver mode
		TEST,           ///< Test / debug mode
		ERROR_IN_EXEC   ///< Legacy password mode or invalid state
	};

	/// <summary>
	/// Encapsulates runtime parameters of the screen saver.
	/// </summary>
	struct ScreensaverParameters final
	{
		friend ScreensaverParameters GetScreensaverParameters() noexcept;

	private:
		/// <summary>
		/// Constructs a screen saver parameter set.
		/// </summary>
		/// <param name="saverMode">Detected execution mode</param>
		/// <param name="wndParent">Parent window (used in preview/config mode)</param>
		ScreensaverParameters(
			const SCREENSAVER_MODE saverMode,
			const HWND wndParent = nullptr
		) noexcept;

	public:
		/// <summary>
		/// Returns true if the screen saver is running in fullscreen mode.
		/// </summary>
		bool IsFullScreen() const noexcept;

		/// <summary>
		/// Current screen saver mode.
		/// </summary>
		const SCREENSAVER_MODE c_screensaverMode;

		/// <summary>
		/// Parent window handle (used for preview/config modes).
		/// </summary>
		const HWND c_hWndParent;
	};

	/// <summary>
	/// Detects and returns screen saver execution parameters.
	/// </summary>
	ScreensaverParameters GetScreensaverParameters() noexcept;
}
