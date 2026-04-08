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
			return findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		}

		inline bool IsSpecialDirectory(const WIN32_FIND_DATA& findFileData) noexcept
		{
			constexpr wstring_view oneDot = L".", twoDots = L"..";
			return oneDot == findFileData.cFileName || twoDots == findFileData.cFileName;
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
	) :
		c_searchFolders{ move(searchFolders) }
	{}

	void FileHandle::Run()
	{
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
				OPER_DEL(HandleHolder);

				const HANDLE c_findFile;
			} handle{ FindFirstFile(
				folder.c_str(),
				&m_findFileData)
			};

			if (INVALID_HANDLE_VALUE != handle.c_findFile)
				do
				{
					if (IsDirectory(m_findFileData))
					{
						if (!IsSpecialDirectory(m_findFileData))
							OnFindDirectory(folder.parent_path());
					}
					else
						OnFindFile(folder.parent_path());
				} while (FindNextFile(
					handle.c_findFile,
					&m_findFileData
				) != FALSE);

				if (const auto dwError = GetLastError();
					ERROR_NO_MORE_FILES != dwError
					&& ERROR_FILE_NOT_FOUND != dwError)
					THROW_EXC_IFFAILED(Exceptions::IOError, HRESULT_FROM_WIN32(dwError), L"Cannot Find Files: " + folder.wstring());
		}
	}

	const WIN32_FIND_DATA& FileHandle::FindFileData() const noexcept
	{
		return m_findFileData;
	}
#pragma endregion
}