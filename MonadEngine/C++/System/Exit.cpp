// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Exit.h"
// STD
#include <atomic>
// Monad
#include "Globals/__MonadGlobals.h"
#include "Exceptions/Exceptions.h"

namespace Monad::System
{
	namespace
	{
		/// <summary>
		/// Global exit code for the application.
		/// Stored atomically to allow safe access across threads.
		/// </summary>
		std::atomic_int g_exitCode = ERROR_SUCCESS;
	}

	/// <summary>
	/// Returns the current application exit code.
	/// </summary>
	/// <returns>
	/// Exit code previously set via SetExitCode().
	/// </returns>
	int32_t GetExitCode() noexcept
	{
		return g_exitCode.load();
	}

	/// <summary>
	/// Sets the application exit code.
	/// The value is written only once – subsequent calls do not overwrite it.
	/// </summary>
	/// <param name="exitCode">
	/// Exit code to be stored.
	/// </param>
	void SetExitCode(
		int32_t exitCode
	) noexcept
	{
		int32_t success = ERROR_SUCCESS;
		g_exitCode.compare_exchange_strong(success, exitCode);
	}

	/// <summary>
	/// Initiates application shutdown sequence.
	/// This function:
	/// - Stores the exit code (only once)
	/// - Hides the main application window
	/// - Posts WM_CLOSE to initiate a clean shutdown
	/// </summary>
	/// <param name="exitCode">
	/// Exit code to be reported when the application terminates.
	/// </param>
	void Shutdown(
		int32_t exitCode
	) noexcept
	{
		SetExitCode(exitCode);

#pragma region Close All Windows
		if (Globals::g_mainWindow)
		{
			// Hide window immediately to avoid visual artifacts
			ShowWindow(Globals::g_mainWindow, SW_HIDE);

			// Request graceful shutdown
			PostMessage(Globals::g_mainWindow, WM_CLOSE, 0, 0);
		}
#pragma endregion
	}
}