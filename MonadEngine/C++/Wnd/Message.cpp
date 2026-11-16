// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Message.h"

// Monad
#include <Globals/__MonadGlobals.h>

using namespace std;

namespace Monad::Wnd
{
	namespace
	{
		void SetMyIcon(
			const HWND hWnd,
			WPARAM wParamSize
		)
		{
			MONAD_FUNCTION_NAME;
			PostMessage(
				hWnd,
				WM_SETICON,
				wParamSize,
				LPARAM(Globals::g_sourceIconHandle)
			);
		}

		ExpectedLResult ProcessWM_ERASEBKGND(
			HWND,
			UINT,
			WPARAM,
			LPARAM,
			LPVOID&
		)
		{
			return PROCESSING_OK;
		}

		ExpectedLResult ProcessWM_SYSKEYDOWN_WM_SYSKEYUP(
			HWND,
			UINT,
			WPARAM wParam,
			LPARAM,
			LPVOID&
		)
		{
			MONAD_FUNCTION_NAME;
			/// Prevents system menu activation via F10
			if (wParam == VK_F10) [[unlikely]]
				return PROCESSING_OK;
			else [[likely]]
				return unexpected(CONTINUE_PROCESSING);
		}

		ExpectedLResult ProcessWM_SYSCOMMAND(
			HWND,
			UINT,
			WPARAM wParam,
			LPARAM,
			LPVOID&
		)
		{
			MONAD_FUNCTION_NAME;
			/// Prevents system menu activation via F10
			if (wParam == VK_F10) [[unlikely]]
				return PROCESSING_OK;
			else [[likely]]
				return unexpected(CONTINUE_PROCESSING);
		}

		ExpectedLResult ProcessWM_DESTROY(
			HWND,
			UINT,
			WPARAM,
			LPARAM,
			LPVOID&
		)
		{
			MONAD_FUNCTION_NAME;
			// Standard shutdown path
			PostQuitMessage(0);
			return PROCESSING_OK;
		}

		ExpectedLResult ProcessWM_CREATE(
			HWND hWnd,
			UINT,
			WPARAM,
			LPARAM,
			LPVOID&
		)
		{
			MONAD_FUNCTION_NAME;
			SetMyIcon(hWnd, ICON_BIG);
			SetMyIcon(hWnd, ICON_SMALL);

			return PROCESSING_OK;
		}
	}

	MapForMessages g_forMessage{
		make_pair(
		WM_SYSKEYDOWN,
		ProcessWM_SYSKEYDOWN_WM_SYSKEYUP
	),
		make_pair(
		WM_SYSKEYUP,
		ProcessWM_SYSKEYDOWN_WM_SYSKEYUP
	),
	MONAD_MSG(
		WM_ERASEBKGND
	),
	MONAD_MSG(
		WM_SYSCOMMAND
	),
	MONAD_MSG(
		WM_DESTROY
	),
	MONAD_MSG(
		WM_CREATE
	)
	};

	ExpectedLResult OnCommonMessages(
		const MapForMessages& mapForMessages,
		const HWND hWnd,
		const UINT uMsg,
		const WPARAM wParam,
		const LPARAM lParam,
		LPVOID& context
	)
	{
		MONAD_FUNCTION_NAME;
		auto msg = mapForMessages.find(uMsg);
		if (mapForMessages.cend() != msg)
			return msg->second(
				hWnd,
				uMsg,
				wParam,
				lParam,
				context
			);
		return unexpected(CONTINUE_PROCESSING);
	}

	ExpectedLResult OnCommonMessages2(
		initializer_list<reference_wrapper<MapForMessages>> mapsForMessages,
		const HWND hWnd,
		const UINT uMsg,
		const WPARAM wParam,
		const LPARAM lParam,
		LPVOID& context
	)
	{
		MONAD_FUNCTION_NAME;
#if defined _DEBUG
		size_t messagesCount = 0;
		MapForMessages summary{};
		for (const auto msg : mapsForMessages)
		{
			messagesCount += msg.get().size();
			summary.insert_range(msg.get());
		}
		assert(summary.size() == messagesCount && "When all are unique");
#endif

		for (const auto msg : mapsForMessages)
			if (auto msgRes =
				OnCommonMessages(
					msg,
					hWnd,
					uMsg,
					wParam,
					lParam,
					context); msgRes.has_value())
				return *msgRes;
		return unexpected(CONTINUE_PROCESSING);
	}
}