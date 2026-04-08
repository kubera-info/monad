// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
#include <roapi.h>
// STD
#include <stdint.h>
// Monad
#include "Dev/Coding.h"
#include "Exceptions/Exceptions.h"
#include "System/Exit.h"
#include "Threads/__MonadThread.h"
#include "System/UniqueInstance.h"

namespace Monad::Kernel
{
	/// <summary>
	/// Entry point helper for Windows-based Monad applications.
	/// Initializes runtime services and executes the main application loop.
	/// </summary>
	template<typename S, typename W>
	inline int32_t MonadWinMain(
		const RECT& rect
	) noexcept
	{
		try
		{
			Globals::g_sourceIconHandle = LoadIcon(
				nullptr != Globals::g_sourceModuleHandle
				? Globals::g_sourceModuleHandle
				: g_hInstance,
				MAKEINTRESOURCE(Globals::g_sourceIcon)
			);

			/*
			// Optional: enforce single instance
			if (System::SmartHandleAmIUnique amIUnique; !amIUnique)
			{
				MessageBeep(MB_ICONERROR);
				System::SetExitCode(EXIT_FAILURE);
			}
			else
			*/
			{
				// Enable modern pointer input (mouse / touch / pen)
				if (FALSE != EnableMouseInPointer(TRUE))
				{
					/// <summary>
					/// Initialize COM runtime for multi-threaded usage.
					/// </summary>
					Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(
						RO_INIT_MULTITHREADED
					);
					THROW_EXC_IFFAILED_RO(initialize);

					// Update render size
					DXSample::UpdateForSizeChange(
						rect.right,
						rect.bottom
					);

					// Application singleton
					S singleton;

					// Enable DXGI adapter removal support
					DXGIDeclareAdapterRemovalSupport();

					// Run application
					System::SetExitCode(W::Run());
				}
				else
				{
					THROW_EXC_GETLASTERROR(
						Monad::Exceptions::NotInitialized,
						L"Mouse"
					);
				}
			}

#if defined(_DEBUG)
			D3D12Core::ReportLiveObjects();
#endif
		}
		MONAD_CATCH;

		return System::GetExitCode();
	}

	/// <summary>
	/// Common initialization macro for Windows applications.
	/// </summary>
#define MONAD_RAW_INIT \
	g_hInstance = hInstance; \
	std::set_terminate(Exceptions::ReportAndTerminate); \
	/* Enable heap corruption termination */ \
	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0); \
	/* Enable per-monitor DPI awareness */ \
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); \
	/* Optional debugging helpers */ \
	/* _crtBreakAlloc = 1528424; */ \
	/* _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); */
}
