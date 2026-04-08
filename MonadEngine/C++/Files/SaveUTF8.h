// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <filesystem>
#include <string>
// Monad
#include "Op.h"

namespace Monad::Files
{
	/// <summary>
	/// Save a UTF8 text to a clear file.
	/// </summary>
	/*void SaveStringAs(
		const std::filesystem::path& filename,
		const std::string& utf8Buffer
	);*/
	HANDLE SaveStringAsWithDACL(
		const std::filesystem::path& path,
		const std::string& utf8Buffer
	);
	template<typename V>
	void SaveStringAs(
		const std::filesystem::path& path,
		const V& buffer
	)
	{
		Files::SmartHandleFile fileToWrite{
					path,
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					CREATE_ALWAYS,
					0
		};
		fileToWrite.Write(buffer);
	}
}
