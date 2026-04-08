// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <stdint.h>

namespace Monad::System
{
	/// <summary>
	/// Returns the current process exit code.
	/// </summary>
	/// <returns>
	/// Exit code of the application, typically ERROR_SUCCESS (0).
	/// </returns>
	int32_t GetExitCode() noexcept;

	/// <summary>
	/// Sets the process exit code.
	/// Only the first call has effect — subsequent calls are ignored.
	/// </summary>
	/// <param name="exitCode">
	/// Exit code to be stored.
	/// </param>
	void SetExitCode(
		int32_t exitCode
	) noexcept;

	/// <summary>
	/// Initiates application shutdown.
	/// The exit code is stored once and any further attempts are ignored.
	/// </summary>
	/// <param name="exitCode">
	/// Exit code returned by the application.
	/// </param>
	void Shutdown(
		int32_t exitCode = ERROR_SUCCESS
	) noexcept;
}