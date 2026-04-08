// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Shlobj.h>
// STD
#include <filesystem>
// Monad
#include "Kernel/Dummy.h"

namespace Monad::Files
{
	/// <summary>
	/// std::path-like class with a constructor that initializes the directory to the application’s assets folder.
	/// </summary>
	struct Path final : std::filesystem::path
	{
		using path::path;
		/// <summary>
		/// A new path for assets.
		/// </summary>
		/// <param name=""Placeholder type to prevent usage of the default std::path constructor./param>
		/// <param name="source">Subdirectories located within the application’s assets folder.</param>
		Path(
			Kernel::DummyParameter,
			const std::filesystem::path& source
		);
		Path& operator=(
			const std::filesystem::path& source
			);
	};

	/// <summary>
	/// Gets a special folder by id
	/// </summary>
	/// <param name="rfid">ID of folder type</param>
	/// <returns>Long path with folder</returns>
	std::filesystem::path GetFolderGeneric(
		const GUID rfid
	);

	/// <summary>
	/// The system profile folder.
	/// </summary>
	std::filesystem::path GetFolderSystem();

	/// <summary>
	/// he application’s binaries folder.
	/// </summary>
	std::filesystem::path GetFolderBin();

	std::filesystem::path GetFolderRes();
	/// <summary>
	/// The user’s profile folder.
	/// (use GET_DOCUMENT_FOLDER instead)
	/// </summary>
	std::filesystem::path GetFolderAppDocumentsSettingsGeneric(
		const std::filesystem::path& profileName
	);
	/// <summary>
	/// The user’s profile folder.
	/// (use GET_DOCUMENT_FOLDER instead)
	/// </summary>
	std::filesystem::path GetFolderAppDocumentsSettings(
		const std::filesystem::path& profileName,
		const std::filesystem::path& folderVersion
	);
#define GET_FOLDER_DOCUMENT Files::GetFolderAppDocumentsSettings(Globals::g_userFolder, APP_VER)

	/// <summary>
	/// Canonical path to the main module (executable).
	/// </summary>
	std::filesystem::path GetModulePath();

	/*void RemoveFolderWithDescendants(
		const std::filesystem::path& path
	);*/
}
