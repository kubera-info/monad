// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Library.h"
// STD
#include <assert.h>
// Monad
#include "Exceptions/Exceptions.h"
#include "Files/Folder.h"

namespace fs = std::filesystem;

namespace Monad::System
{
	namespace
	{
		const HMODULE Validate(
			const fs::path& path
		)
		{
			const HMODULE lib = ::LoadLibraryEx(
				path.c_str(),
				nullptr,
				LOAD_WITH_ALTERED_SEARCH_PATH
			);
			if (nullptr == lib)
				THROW_EXC_GETLASTERROR(
					Exceptions::InvalidClass,
					L"Load LibraryEx: " + path.wstring()
				);
			return lib;
		}
	}
#pragma region LibraryGeneric
	inline LibraryGeneric::LibraryGeneric(
		const fs::path& folder,
		const wchar_t* fileName
	) :
		m_lib(Validate((folder / fileName)))
	{}

	LibraryGeneric::~LibraryGeneric()
	{
		FreeLibrary(m_lib);
	}

	FARPROC WINAPI LibraryGeneric::GetProcAddress(
		const char* procedureName
	) const noexcept
	{
		return ::GetProcAddress(m_lib, procedureName);
	}
#pragma endregion

#pragma region SystemLibrary
	SystemLibrary::SystemLibrary(
		const wchar_t* fileName
	) :
		LibraryGeneric(
			Files::GetFolderSystem(),
			fileName)
	{}
#pragma endregion

#pragma region LocalLibrary
	LocalLibrary::LocalLibrary(
		const wchar_t* fileName
	) :
		LibraryGeneric(
			Files::GetFolderBin(),
			fileName
		)
	{}
#pragma endregion
}