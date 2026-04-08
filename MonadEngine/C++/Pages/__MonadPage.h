// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

namespace Monad::Pages
{
	extern std::string g_currentPage;
}

constexpr std::string operator""_pageNo(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}