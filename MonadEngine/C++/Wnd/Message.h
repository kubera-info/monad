// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <expected>
#include <memory>
#include <map>

namespace Monad::Wnd
{
	// =========================================================================
	// Message handling helpers
	// =========================================================================

	/// <summary>
	/// Result of message processing.
	/// </summary>
	enum LRESULT_STATUS : LRESULT
	{
		PROCESSING_OK = 0,
		PROCESSING_ERROR = -1,
		PROCESSING_TRUE = 1
	};

	/// <summary>
	/// Signals that default window procedure should continue processing.
	/// </summary>
	enum LRESULT_CONTINUE : LRESULT
	{
		CONTINUE_PROCESSING = 1
	};

	/// <summary>
	/// Result type for window message processing.
	/// 
	/// ✔ value   → message handled  
	/// ✘ error   → continue with DefWindowProc
	/// </summary>
	using ExpectedLResult = std::expected<LRESULT_STATUS, LRESULT_CONTINUE>;

	using MessageProcess = ExpectedLResult(*)(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		LPVOID& context
		);

	using MapForMessages = std::map<uint32_t, MessageProcess>;
	extern MapForMessages g_forMessage;

	/// <summary>
	/// Common handler for shared window messages.
	/// Allows central filtering of system messages.
	/// </summary>
	ExpectedLResult OnCommonMessages(
		const MapForMessages& mapForMessages,
		const HWND hWnd,
		const UINT uMsg,
		const WPARAM wParam,
		const LPARAM lParam,
		LPVOID& context
	);
	ExpectedLResult OnCommonMessages2(
		std::initializer_list<std::reference_wrapper<MapForMessages>> mapsForMessages,
		const HWND hWnd,
		const UINT uMsg,
		const WPARAM wParam,
		const LPARAM lParam,
		LPVOID& context
	);
}
#define MONAD_MSG(WM) \
make_pair(\
	WM,\
	Process##WM\
)