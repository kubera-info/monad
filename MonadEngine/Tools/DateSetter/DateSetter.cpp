// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "AppFileTimestamp.h"
#include "Files/__MonadFile.h"
#include "Files/Handle.h"
#include "Globals/__MonadGlobals.h"

namespace fs = std::filesystem;

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
			if (const auto file = selectedFolder / FindFileData().cFileName; Monad::Files::IsMonadExt(file.extension()))
				SetAssetFileTime(file);
		}

		void OnFindDirectory(
			const std::filesystem::path& selectedFolder
		) final
		{
			FileHandleTime{ { selectedFolder / FindFileData().cFileName / Monad::Files::WILDCARD_ANY } }.Run();
		}

	} handle{ { folder } };
	handle.Run();
}
int wmain(
	int argc,
	wchar_t* wargv[]
)
{
	std::wcout << L"Setting files dates\n";
	if (2 != argc)
	{
		std::wcerr << L"Error: Argument should be: a path to the Monad(a) Resources\n";
		MessageBeep(MB_ICONERROR);
		return EXIT_FAILURE;
	}

	Monad::Globals::g_exceptionReportingMethod = Monad::Exceptions::EXCEPTION_PRESENTATION::REPORT_TO_CONSOLE;

	const fs::path startingFolder(wargv[1]);
	if (!(std::filesystem::is_directory(wargv[1]) && startingFolder.generic_string().back() != L'\\'))
	{
		std::wcerr << L"Error: Argument should be a correct path to a folder and without trailing \\, is: " << wargv[1] << "\n";
		MessageBeep(MB_ICONERROR);
		return EXIT_FAILURE;
	}
	IterateFiles(startingFolder);
	return ERROR_SUCCESS;
}