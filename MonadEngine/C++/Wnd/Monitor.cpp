// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Monitor.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Wnd
{
	namespace
	{
		/// <summary>
		/// Helper structure used internally to collect non-primary monitors.
		/// Inherits from SetMonitors and stores the primary monitor handle
		/// to allow filtering during enumeration.
		/// </summary>
		struct SetNonPrimaryMonitors : SetMonitors
		{
			BOOL AddNonPrimaryMonitor(
				const HMONITOR hMonitor,
				const LPRECT lprcMonitor
			)
			{
				// Skip primary monitor
				if (m_hMonitor != hMonitor)
				{
					try
					{
						emplace(*lprcMonitor);
					}
					catch (const std::bad_alloc&)
					{
						// Stop enumeration if allocation fails
						return FALSE;
					}
				}
				return TRUE;
			}
			/// <summary>
			/// Handle to the primary monitor.
			/// Used to exclude it from the enumeration results.
			/// </summary>
			const HMONITOR m_hMonitor = GetPrimaryMonitor();
		};
	}

	/// <summary>
	/// Returns handle to the system primary monitor.
	/// </summary>
	/// <returns>HMONITOR of the primary display</returns>
	HMONITOR GetPrimaryMonitor() noexcept
	{
		// (0,0) is always guaranteed to be on the primary monitor
		return MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
	}

	namespace
	{
		/// <summary>
		/// Callback used by EnumDisplayMonitors.
		/// Collects all non-primary monitors into SetNonPrimaryMonitors container.
		/// </summary>
		BOOL CALLBACK RegisterMonitor(
			_In_  HMONITOR hMonitor,
			_In_  HDC,
			_In_  LPRECT lprcMonitor,
			_In_  LPARAM dwData
		) noexcept
		{
			return reinterpret_cast<SetNonPrimaryMonitors*>(dwData)->
				AddNonPrimaryMonitor(hMonitor, lprcMonitor);
		}
	}

	/// <summary>
	/// Returns the rectangle of the given monitor in virtual screen coordinates.
	/// </summary>
	/// <param name="monitor">Monitor handle</param>
	/// <returns>Rectangle describing the monitor area</returns>
	RECT GetMonitorRect(
		HMONITOR monitor
	)
	{
		MONITORINFO mi{ sizeof mi };

		if (!GetMonitorInfo(monitor, &mi))
			THROW_EXC_GETLASTERROR(
				Exceptions::InvalidCall,
				L"Get Monitor Info"
			);

		return mi.rcMonitor;
	}

	/// <summary>
	/// Enumerates all monitors except the primary one.
	/// </summary>
	/// <returns>
	/// A set containing rectangles of all non-primary monitors.
	/// </returns>
	SetMonitors GetNonPrimaryMonitors()
	{
		SetNonPrimaryMonitors nonPrimaryMonitors;

		EnumDisplayMonitors(
			nullptr,
			nullptr,
			RegisterMonitor,
			reinterpret_cast<LPARAM>(&nonPrimaryMonitors)
		);

		return nonPrimaryMonitors;
	}
}