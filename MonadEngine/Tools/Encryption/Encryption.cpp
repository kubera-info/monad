// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "stdafx.h"
#include "Files/EncryptAndSave.h"
#include "Files/__MonadFile.h"
#include "Globals/__MonadGlobals.h"
#include "Tools/Convert.h"

using namespace std;
namespace fs = std::filesystem;
using namespace Monad::Files;

/// <summary>
/// Converts assets to compressed and encrypted files
/// </summary>
/// <param name="cipher">Cipher context</param>
/// <param name="pInputPath">Input file path</param>
/// <param name="outputPath">Output file path</param>
inline void CompressEncryptAndSaveFromFile(
	const fs::path& inputPath,
	const fs::path& outputPath
)
{
	const SmartHandleFile fileForReading(
		inputPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		0
	);
	Monad::Kernel::VectorBytes memoryBuffer(fileForReading.FileSize());
	fileForReading.Read(memoryBuffer);
	CompressEncryptAndSaveFromMemory(
		g_cipher,
		outputPath,
		memoryBuffer		
	);
}

int _tmain(
	int argc,
	wchar_t* argv[]
)
{
	if (4 != argc)
	{
		MessageBeep(MB_ICONERROR);
		printf("Error: The wrong number of parameters (%d)\n", argc);
		return 1;
	}
	Monad::Globals::g_exceptionReportingMethod = Monad::Exceptions::EXCEPTION_PRESENTATION::MESSAGE_BOX;
	Monad::Globals::g_sha256Salt = Monad::Tools::UnicodeToUTF8(argv[1]);

	try
	{
		CompressEncryptAndSaveFromFile(
			argv[2],
			argv[3]
		);
	}
	MONAD_CATCH;
	if (!Monad::Exceptions::IsValid())
		return 2;
	wprintf(
		L"Compressed: %s\n",
		argv[2]
	);

	return ERROR_SUCCESS;
}