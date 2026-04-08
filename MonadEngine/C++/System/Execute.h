// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
// Monad
#include "Parameter.h"

namespace Monad::System
{
	/// <summary>
	/// Executes an external application.
	/// </summary>
	/// <param name="path">
	/// Path to the executable file.
	/// </param>
	/// <param name="parameters">
	/// Command-line parameters passed to the application.
	/// </param>
	/// <param name="waitForProcessEnd">
	/// If true, the function blocks until the launched process terminates.
	/// </param>
	/// <returns>
	/// True if the process was successfully created, false otherwise.
	/// </returns>
	bool ExecuteApp(
		std::filesystem::path path,
		std::wstring parameters,
		bool waitForProcessEnd = false
	);

	/// <summary>
	/// Executes an external application using a parameter list.
	/// </summary>
	/// <param name="path">
	/// Path to the executable file.
	/// </param>
	/// <param name="parameters">
	/// List of parameters which will be converted into a command-line string.
	/// </param>
	/// <param name="waitForProcessEnd">
	/// If true, the function blocks until the launched process terminates.
	/// </param>
	/// <returns>
	/// True if the process was successfully created, false otherwise.
	/// </returns>
	bool ExecuteApp(
		std::filesystem::path path,
		InitializerListParameters parameters,
		bool waitForProcessEnd = false
	);
}