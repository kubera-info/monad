// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Handle.h"

// Monad
#include "Exceptions/Exceptions.h"

namespace fs = std::filesystem;
using namespace std;

namespace Monad::Files
{
	namespace
	{
		inline bool IsDirectory(const WIN32_FIND_DATA& findFileData) noexcept
		{
			MONAD_FUNCTION_NAME;
			return findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		}

		inline bool IsSpecialDirectory(const wchar_t* cFileName) noexcept
		{
			MONAD_FUNCTION_NAME;
			constexpr wstring_view oneDot = L".", twoDots = L"..";
			return oneDot == cFileName || twoDots == cFileName;
		}
	}
#pragma region FileHandle
	FileHandle::FileHandle(
		const Kernel::VectorPath& searchFolders
	) :
		c_searchFolders{ searchFolders }
	{}

	FileHandle::FileHandle(
		Kernel::VectorPath&& searchFolders
	) noexcept :
		c_searchFolders{ move(searchFolders) }
	{}

	void FileHandle::Run()
	{
		MONAD_FUNCTION_NAME;
		WIN32_FIND_DATA findFileData{};
		for (const auto& folder : c_searchFolders)
		{
			struct HandleHolder final
			{
				inline HandleHolder(
					const HANDLE findFile
				) noexcept :
					c_findFile{ findFile }
				{}
				inline ~HandleHolder()
				{
					if (INVALID_HANDLE_VALUE != c_findFile)
						FindClose(c_findFile);
				}
				HandleHolder() = delete;
				HandleHolder(const HandleHolder&) = delete;
				HandleHolder& operator=(const HandleHolder&) = delete;
				HandleHolder(HandleHolder&&) noexcept = delete;
				HandleHolder& operator=(HandleHolder&&) noexcept = delete;

				const HANDLE c_findFile;
			} handle{ FindFirstFile(
				folder.c_str(),
				&findFileData)
			};

			if (INVALID_HANDLE_VALUE != handle.c_findFile)
				do
				{
					if (IsDirectory(findFileData))
					{
						if (!IsSpecialDirectory(findFileData.cFileName))
							OnFindDirectory(folder.parent_path(), findFileData);
					}
					else
						OnFindFile(folder.parent_path(), findFileData);
				} while (FindNextFile(
					handle.c_findFile,
					&findFileData
				) != FALSE);

				if (const auto dwError = GetLastError();
					ERROR_NO_MORE_FILES != dwError
					/* && ERROR_FILE_NOT_FOUND != dwError*/)
					THROW_EXC_IFFAILED(Exceptions::IOError, HRESULT_FROM_WIN32(dwError), L"Cannot Find Files: " + folder.wstring());
		}
	}
#pragma endregion
}