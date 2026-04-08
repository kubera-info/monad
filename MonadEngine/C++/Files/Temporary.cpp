// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Temporary.h"
// Monad
#include "Tools/Convert.h"

namespace fs = std::filesystem;

namespace Monad::Files
{
	fs::path GlobalFileNameSuffix()
	{
		Tools::OutputStringStream strmForFolderName;

		SYSTEMTIME currentTime;
		GetLocalTime(&currentTime);
		strmForFolderName << L"MonadaEngine" << currentTime;
		return strmForFolderName.str();
	}

	/*FolderGeneric::FolderGeneric(
		const fs::path& folder
	) :
		c_folder{ folder }
	{
		fs::create_directory(folder);
	}

	fs::path FolderGeneric::GetPathForFile(
		const fs::path& filename
	)
	{
		return c_folder / filename;
	}

	void FolderGeneric::DeleteFileByName(
		const fs::path& filename
	)
	{
		DeleteFile(GetPathForFile(filename).c_str());
	}

	TemporaryFolder::TemporaryFolder() :
		FolderGeneric{ fs::temp_directory_path() / GlobalFileNameSuffix() },
		c_me(
			this,
			g_temporaryFolder
		)
	{
	}

	TemporaryFolder::~TemporaryFolder()
	{
		if (Exceptions::IsValid())
			RemoveFolderWithDescendants(c_folder);
	}
	TemporaryFolder* g_temporaryFolder = nullptr;*/
}