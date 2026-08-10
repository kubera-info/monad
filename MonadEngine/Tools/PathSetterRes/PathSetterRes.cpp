// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include <iostream>
#include <map>
#include <set>
#include <array>
#include "Files/__MonadFile.h"
#include "Files/Handle.h"
#include "Kernel/GUID.h"
#include "Kernel/Templates.h"
#include "Tools/Convert.h"

namespace fs = std::filesystem;
using namespace Monad::Kernel;

namespace
{
	void IterateFiles(
		const fs::path& folder
	)
	{
		struct FileHandleTime : Monad::Files::FileHandle
		{
			using FileHandle::FileHandle;

			void OnFindFile(
				const std::filesystem::path& selectedFolder
			) final
			{
				fs::path currentFolder = selectedFolder / FindFileData().cFileName, relPath = FindFileData().cFileName;
				if (Monad::Files::IsMonadExt(relPath.extension()))
				{
					do
					{
						currentFolder = currentFolder.parent_path();
						relPath = currentFolder.filename() / relPath;
					} while (Monad::Tools::ToLower(currentFolder.filename()) != L"res");

					wprintf(L"\n  <Content Include=\"..\\%s\">", relPath.c_str());
					wprintf(L"\n    <Link>%s</Link>", relPath.c_str());
					wprintf(L"\n  </Content>");
				}
			}

			void OnFindDirectory(
				const std::filesystem::path& selectedFolder
			) final
			{
				FileHandleTime{ { selectedFolder / FindFileData().cFileName } }.Run();
			}

		} handle{ { folder } };
		handle.Run();
	}

	constexpr std::wstring_view replaceRes = L"Res\\", replaceID = L"\\", replaceMinus = L"-", replaceDots = L"..", replaceAp = L"'";
	constexpr std::wstring_view replaceMinus2 = L"_";

	std::map<std::wstring, std::set<std::wstring>> g_Paths;

	void IterateFilesMSI(
		const fs::path& folder
	)
	{
		struct FileHandleTime : Monad::Files::FileHandle
		{
			using FileHandle::FileHandle;
			void OnFindFile(
				const std::filesystem::path& selectedFolder
			) final
			{
				std::wstring filename = FindFileData().cFileName;
				filename[0] = towupper(filename[0]);
				filename = L"__" + filename;
				fs::path currentFolder = selectedFolder / FindFileData().cFileName, relPath = FindFileData().cFileName, relPath2 = filename;
				if (Monad::Files::IsMonadExt(relPath.extension()))
				{
					do
					{
						currentFolder = currentFolder.parent_path();
						relPath = currentFolder.filename() / relPath;
						relPath2 = currentFolder.filename() / relPath2;
					} while (Monad::Tools::ToLower(currentFolder.filename()) != L"res");
					std::wstring tagID = relPath2.wstring();
					ReplaceAll(tagID, replaceRes);
					ReplaceAll(tagID, replaceID);
					ReplaceAll(tagID, replaceMinus, replaceMinus2);
					ReplaceAll(tagID, replaceAp, replaceMinus2);
					std::wstring buf = L"\n			<File ID=\"";
					buf += tagID;
					buf += L"\" Source=\"..\\";
					buf += relPath;
					buf += L"\" />";
					g_Paths[selectedFolder].emplace(buf);
				}
			}
			void OnFindDirectory(
				const std::filesystem::path& selectedFolder
			) final
			{
				FileHandleTime{ { selectedFolder / FindFileData().cFileName } }.Run();
			}
		} handle{ { folder } };
		handle.Run();
	}

	void Scis()
	{
		wprintf(L"\n\n 8X========\n\n");
		wprintf(L"\n\n 8X========\n\n");
		wprintf(L"\n\n 8X========\n\n");
	}
}

int wmain(
	int argc,
	wchar_t* wargv[]
)
{
	std::wcout << L"Processing Folders and Files\n";
	if (2 != argc)
	{
		std::wcerr << L"Error: Argument should be: a path to the Monad(a) Resources\n";
		MessageBeep(MB_ICONERROR);
		return EXIT_FAILURE;
	}
	const fs::path startingFolder(wargv[1]);
	if (!(fs::is_directory(wargv[1]) && startingFolder.generic_string().back() != L'\\'))
	{
		std::wcerr << L"Error: Argument should be a correct path to a folder and without trailing \\, is: " << wargv[1] << "\n";
		MessageBeep(MB_ICONERROR);
		return EXIT_FAILURE;
	}
	Scis();
	IterateFiles(startingFolder);
	Scis();
	IterateFilesMSI(startingFolder);
	for (const auto& fld : g_Paths)
	{
		auto key = fld.first;
		ReplaceAll(key, replaceRes);
		ReplaceAll(key, replaceID);
		ReplaceAll(key, replaceDots);
		ReplaceAll(key, replaceMinus);
		wprintf(L"\n	<ComponentGroup ID=\"%s\" Directory=\"%s\">", (key + L"ComGrp").c_str(), (key + L"Folder").c_str());

		wprintf(L"\n	<Component ID=\"%s\" Guid=\"{%s}\">", key.c_str(), GetGUID().c_str());
		for (const auto& s : fld.second)
			wprintf(L"%s", s.c_str());
		wprintf(L"\n	</Component>\n</ComponentGroup>");
	}
	Scis();
	return ERROR_SUCCESS;
}