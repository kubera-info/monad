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
		const wregex REGEX_MONAD_EXT(L"^\\.monad\\-[a-z]{3}$", std::regex_constants::icase);
	}

	bool IsMonadExt(
		const fs::path& fileExt
	)
	{
		return regex_match(fileExt.c_str(), REGEX_MONAD_EXT);
	}
}	