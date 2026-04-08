// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "stdafx.h"
#include "Timestamp.h"
//
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

namespace Monad::Dev
{
	namespace
	{
		FILETIME GetCurrentFTime(
			const SYSTEMTIME& st
		) noexcept
		{
			FILETIME ft;
			SystemTimeToFileTime(&st, &ft);

			return ft;
		}

		BOOL SetFileTime(
			HANDLE file,
			const SYSTEMTIME& st
		) noexcept
		{
			const FILETIME ft(GetCurrentFTime(st));

			return SetFileTime(file,           // sets last-write time for file
				&ft, &ft, &ft);
		}
	}
	void SetFileTime(
		const fs::path& fileName,
		const SYSTEMTIME& st
	) noexcept
	{
		if (const HANDLE file = CreateFile2(fileName.c_str(),
			GENERIC_WRITE,
			0,
			OPEN_EXISTING,
			nullptr); INVALID_HANDLE_VALUE != file)
		{
			const FILETIME ft(GetCurrentFTime(st));

			if (FALSE == SetFileTime(file, st))
				wcerr << L"Error: Can not set a timestamp #2\n";
			else
				wcout << fileName << L' ';
		}
		else
			wcerr << L"Error: Can not set a timestamp\n";
	}
}