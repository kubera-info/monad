// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>
#include <filesystem>
// Monad
#include "Kernel/Del.h"
#include "Kernel/Templates.h"

namespace Monad::Files
{
	std::filesystem::path GlobalFileNameSuffix();

	/*interface FolderGeneric abstract
	{
		std::filesystem::path GetPathForFile(
			const std::filesystem::path & filename
		);
		void DeleteFileByName(
			const std::filesystem::path& filename
		);
		FolderGeneric(
			const std::filesystem::path& folder
		);
		OPER_DEL(FolderGeneric);

	protected:
		const std::filesystem::path c_folder;
	};

	struct TemporaryFolder : FolderGeneric
	{
		TemporaryFolder();
		OPER_DEL_NO_DEF_CTOR(TemporaryFolder);
		~TemporaryFolder();

		const Kernel::Me<TemporaryFolder> c_me;
	} extern* g_temporaryFolder;*/
}