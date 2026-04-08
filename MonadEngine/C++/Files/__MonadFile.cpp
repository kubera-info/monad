// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadFile.h"
// STD
#include <regex>

using namespace std;
namespace fs = std::filesystem;

namespace Monad::Files
{
	const fs::path
		PROFILE_FILENAME(L"profile.monad-dbf"),
		WILDCARD(L"*.monad-???"),
		WILDCARD_ANY(L"*");
	namespace
	{
		const wregex REGEX_MONAD_EXT(L"^\\.monad\\-[a-z]{3}$");
	}

	bool IsMonadExt(
		const wstring& fileExt
	)
	{
		return regex_match(fileExt, REGEX_MONAD_EXT);
	}
}