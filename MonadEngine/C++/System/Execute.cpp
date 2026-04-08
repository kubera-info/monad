// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Execute.h"
// Monad
#include "Exceptions\Exceptions.h"

using namespace std;
namespace fs = std::filesystem;

namespace Monad::System
{
	/// <summary>
	/// Executes an external application.
	/// </summary>
	/// <param name="path">
	/// Full path to the executable to be launched.
	/// </param>
	/// <param name="parameters">
	/// Command-line parameters passed to the process.
	/// </param>
	/// <param name="waitForProcessEnd">
	/// If true, the function blocks until the process terminates.
	/// </param>
	/// <returns>
	/// True if the process was successfully created,
	/// false otherwise.
	/// </returns>
	bool ExecuteApp(
		fs::path path,
		std::wstring parameters,
		bool waitForProcessEnd
	)
	{
		STARTUPINFO si{ .cb = sizeof STARTUPINFO };
		PROCESS_INFORMATION pi{};

		// Create a new process using the Win32 API.
		if (FALSE == CreateProcess(
			path.c_str(),
			parameters.data(),
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			TRUE,			// Handle inheritance
			0,              // No special creation flags
			NULL,           // Inherit environment
			NULL,           // Inherit working directory
			&si,            // Startup configuration
			&pi))           // Process information
		{
			THROW_EXC_GETLASTERROR(
				Exceptions::FileDamaged,
				L"Create Process"
			);
		}

		bool processStarted = false;

		// Optionally wait for process termination
		if (waitForProcessEnd)
			WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles
		if (pi.hProcess)
		{
			CloseHandle(pi.hProcess);
			processStarted = true;
		}

		if (pi.hThread)
			CloseHandle(pi.hThread);

		return processStarted;
	}

	/// <summary>
	/// Executes an external application using a parameter list.
	/// </summary>
	/// <param name="path">
	/// Full path to the executable.
	/// </param>
	/// <param name="parameters">
	/// List of parameters that will be converted into a command-line string.
	/// </param>
	/// <param name="waitForProcessEnd">
	/// If true, blocks until the launched process exits.
	/// </param>
	/// <returns>
	/// True if the process was successfully created.
	/// </returns>
	bool ExecuteApp(
		fs::path path,
		InitializerListParameters parameters,
		bool waitForProcessEnd
	)
	{
		return ExecuteApp(
			path,
			CreateParameters(parameters),
			waitForProcessEnd
		);
	}
}