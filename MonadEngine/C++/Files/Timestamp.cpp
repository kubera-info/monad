// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "stdafx.h"
#include "Timestamp.h"
//
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

namespace Monad::Files
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
	)
	{
		FILETIME ft;
		CREATEFILE2_EXTENDED_PARAMETERS
			par_c = {
			sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
			FILE_ATTRIBUTE_NORMAL,
			FILE_FLAG_BACKUP_SEMANTICS };

		SystemTimeToFileTime(&st, &ft);
		if (const HANDLE file = CreateFile2(fileName.c_str(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			OPEN_EXISTING,
			&par_c); INVALID_HANDLE_VALUE != file)
		{
			if (FALSE == SetFileTime(file,           // sets last-write time for file
				&ft, &ft, &ft))
			{
				THROW_EXC_GETLASTERROR(Monad::Exceptions::IOError, L"SetFileTime");
			}
		}
		else
			THROW_EXC_GETLASTERROR(Monad::Exceptions::IOError, L"CreateFile2");
	}

}