// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "SaveUTF8Encrypted.h"
// Monad
#include "Op.h"
#include "EncryptAndSave.h"

using namespace std;
namespace fs = std::filesystem;

namespace Monad::Files
{
	void SaveStringAs(
		Safety::SafetyState& cipher,
		const fs::path& filename,
		const string& utf8Buffer
	)
	{
		RenameFileForBackup(filename);
		CompressEncryptAndSaveFromMemory(
			cipher,
			filename,
			utf8Buffer
		);
	}
}