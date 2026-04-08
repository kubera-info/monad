// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

namespace Monad::Time
{
	/// <summary>
	/// Global time counter expressed in seconds.
	///
	/// Represents the elapsed time since the rendering system was started.
	/// The value may be paused or frozen depending on application state
	/// (e.g. pause menu, minimized window, debugger stop, etc.).
	///
	/// This value is typically used for:
	/// - animations
	/// - time-based interpolation
	/// - transitions and effects
	/// - frame-independent logic
	/// </summary>
	extern double g_fTime;
}