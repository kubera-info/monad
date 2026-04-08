// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Templates.h"

namespace Monad::Wnd
{
	/// <summary>
	/// A set of rectangles describing non-primary monitors.
	/// Each rectangle represents the desktop coordinates of a monitor.
	/// </summary>
	using SetMonitors = Kernel::UnorderedSet<RECT>;

	/// <summary>
	/// Retrieves the handle of the primary monitor.
	/// </summary>
	/// <returns>
	/// Handle to the primary display monitor.
	/// </returns>
	HMONITOR GetPrimaryMonitor() noexcept;

	/// <summary>
	/// Retrieves the desktop rectangle of a given monitor.
	/// </summary>
	/// <param name="monitor">
	/// Handle to the monitor.  
	/// If omitted, the primary monitor is used.
	/// </param>
	/// <returns>
	/// Rectangle describing the monitor position in virtual screen space.
	/// </returns>
	RECT GetMonitorRect(
		HMONITOR monitor = GetPrimaryMonitor()
	);

	/// <summary>
	/// Enumerates all monitors except the primary one.
	/// </summary>
	/// <returns>
	/// A set containing rectangles of all non-primary monitors.
	/// </returns>
	SetMonitors GetNonPrimaryMonitors();
}