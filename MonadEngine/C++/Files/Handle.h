// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <shlwapi.h>
// Monad
#include "Kernel/Del.h"
#include "Kernel/Patterns.h"

namespace Monad::Files
{
	/// <summary>
	/// Smart handle for locating files.
	/// </summary>
	interface FileHandle abstract
	{
		FileHandle(
			const Kernel::VectorPath & searchFolders
		);
		FileHandle(
			Kernel::VectorPath&& searchFolders
		);
		OPER_DEL(FileHandle);
		void Run();

	protected:
		virtual void OnFindFile(
			[[maybe_unused]] const std::filesystem::path& selectedFolder
		)
		{}
		virtual void OnFindDirectory(
			[[maybe_unused]] const std::filesystem::path& selectedFolder
		)
		{}
		const WIN32_FIND_DATA& FindFileData() const noexcept;

	private:
		const Kernel::VectorPath c_searchFolders;
		WIN32_FIND_DATA m_findFileData{};
	};
}
