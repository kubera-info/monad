// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
// STD
#include <print>
// Monad
#include "Exceptions/Exceptions.h"
#include "Globals/__MonadGlobals.h"
#include "Files/__MonadFile.h"
#include "Files/Handle.h"
#include "Tools/Convert.h"

namespace fs = std::filesystem;

int wmain(
	int argc,
	wchar_t* argv[]
)
{
	if (argc != 2)
	{
		std::println("Error: Invalid count of arguments, please enter the start folder.");
		return EXIT_FAILURE;
	}
	std::println("Change extensions started!");

	Monad::Globals::g_exceptionReportingMethod = Monad::Exceptions::EXCEPTION_PRESENTATION::REPORT_TO_CONSOLE;

	try
	{
		struct FileHandleLowerCase : Monad::Files::FileHandle
		{
			using FileHandle::FileHandle;

			void OnFindDirectory(const fs::path& selectedFolder) final
			{
				FileHandleLowerCase{ { selectedFolder / FindFileData().cFileName / Monad::Files::WILDCARD_ANY } }.Run();
			}
			void OnFindFile(const fs::path& selectedFolder) final
			{
				const fs::path
					p = selectedFolder / FindFileData().cFileName,
					e = p.extension();
				const std::wstring n = Monad::Tools::ToLower(e.wstring());
				const fs::path s = p.parent_path() / (p.stem().wstring() + n);
				if (e != n && Monad::Tools::ToLower(p) == Monad::Tools::ToLower(s))
					fs::rename(p, s);
			}
		} fileEXT2ext{ { argv[1] } };
		fileEXT2ext.Run();
	}
	MONAD_CATCH;
}