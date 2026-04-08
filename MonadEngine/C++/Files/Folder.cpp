// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Folder.h"
// Platform
#pragma comment(lib, "OneCore.Lib")

// Monad
#include "__MonadFile.h"
#include "Kernel/Del.h"
#include "Exceptions/Exceptions.h"

using namespace std;
namespace fs = std::filesystem;
using namespace std::string_literals;

namespace Monad::Files
{
	fs::path GetFolderGeneric(
		const GUID rfid
	)
	{
		struct Mem
		{
			Mem() noexcept = default;
			~Mem()
			{
				// nullptr is OK for this subprogram
				CoTaskMemFree(m_str);
			}
			OPER_DEL_NO_DEF_CTOR(Mem);

			wchar_t* m_str = nullptr;
		} folder;
		THROW_EXC_IFFAILED(Exceptions::FileNotFound, SHGetKnownFolderPath(
			rfid,
			KF_FLAG_DEFAULT,
			nullptr,
			&folder.m_str
		), L"Get Known Folder Path");
		return folder.m_str;
	}

	fs::path GetFolderSystem()
	{
		return GetFolderGeneric(FOLDERID_System);
	}

	fs::path GetModulePath()
	{
		wchar_t pathModule[32767ul];
		THROW_EXC_IFFALSE(Monad::Exceptions::IOError, GetModuleFileName(
			nullptr,
			pathModule,
			32767ul
		), L"FS Path");
		return pathModule;
	}

	fs::path GetFolderAppDocumentsSettingsGeneric(
		const fs::path& profileName
	)
	{
		if (!profileName.empty())
		{
			fs::path profileFolder = GetFolderGeneric(FOLDERID_Documents) / profileName;
			fs::create_directories(profileFolder);
			return profileFolder;
		}
		else
			return L"";
	}

	fs::path GetFolderAppDocumentsSettings(
		const fs::path& profileName,
		const fs::path& folderVersion
	)
	{
		if (!profileName.empty())
		{
			fs::path profileFolder = GetFolderAppDocumentsSettingsGeneric(profileName) / folderVersion;
			fs::create_directories(profileFolder);
			return profileFolder;
		}
		else
			return L"";
	}

	fs::path GetFolderRes()
	{
		return GetFolderBin().parent_path() / L"Res"s;
	}

	Path::Path(
		Kernel::DummyParameter,
		const fs::path& source
	) :
		path{ Files::GetFolderRes() / source }
	{}

	Path& Path::operator=(
		const fs::path& source
		)
	{
		path::operator=(source);
		return *this;
	}

	fs::path GetFolderBin()
	{
		return GetModulePath().parent_path();
	}

	/*void RemoveFolderWithDescendants(const fs::path& path)
	{
		if (fs::exists(path) && fs::is_directory(path))
		{
			for (const auto& entry : fs::directory_iterator(path))
				fs::remove_all(entry.path());
			fs::remove(path);
		}
	}*/
}