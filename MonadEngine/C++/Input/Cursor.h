// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <atomic>
// Monad
#include "GUI/GUIMath.h"

struct DXSample;

namespace Monad::Input
{
	/// <summary>
	/// Indicates whether the input system is currently active.
	/// </summary>
	extern std::atomic_bool g_active;

	/// <summary>
	/// Supported cursor types.
	/// </summary>
	enum class CURSOR_TYPES : size_t
	{
		HOUR_GLASS,
		POINTER
	};

	/// <summary>
	/// Current cursor type.
	/// </summary>
	extern std::atomic<CURSOR_TYPES> g_cursorType;

	/// <summary>
	/// Current cursor position in screen space.
	/// </summary>
	extern DirectX::XMFLOAT2 g_cursorPos;

	/// <summary>
	/// System cursor handles.
	/// </summary>
	extern const HCURSOR 
		CURSOR_WAIT,
		CURSOR_NO,
		CURSOR_ARROW;

	/// <summary>
	/// Centers the cursor on the screen.
	/// </summary>
	void CenterCursor() noexcept;

	/// <summary>
	/// Sets the cursor position using explicit coordinates.
	/// </summary>
	void SetCursorPosGeneric(
		const long x,
		const long y
	) noexcept;

	/// <summary>
	/// Sets the cursor position using a Windows LPARAM value.
	/// </summary>
	void SetCursorPos(
		const LPARAM lParam
	) noexcept;

	/// <summary>
	/// Manages cursor state and updates the active cursor.
	/// </summary>
	struct CursorControl
	{
		/// <summary>
		/// Sets a new current cursor.
		/// </summary>
		/// <param name="newCursor">Handle to the new cursor.</param>
		void SetNewCurrentCursor(
			HCURSOR newCursor
		) noexcept;

		/// <summary>
		/// Applies the currently stored cursor.
		/// </summary>
		void SetCurrentCursor() const noexcept;

	private:
		/// <summary>
		/// Currently selected cursor handle.
		/// </summary>
		HCURSOR m_currentCursor = nullptr;
	};
}
