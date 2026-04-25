// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "SaveUTF8.h"

using namespace std;
namespace fs = std::filesystem;

namespace Monad::Files
{
	HANDLE SaveStringAsWithDACL(
		const fs::path& path,
		const string& utf8Buffer
	)
	{
		SmartHandleFile fileToWrite{
			path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			g_safeAccessParameters.lpSecurityAttributes,
			CREATE_ALWAYS,
			g_safeAccessParameters.dwFileAttributesAndFlags
		};
		fileToWrite.Write(utf8Buffer);
		fileToWrite.SetFilePointerToBegin();
		return fileToWrite.Detach();
	}
}