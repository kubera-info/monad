// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#define NOMINMAX
#define NODRAWTEXT
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <iostream>

void SetAssetFileTime(const std::filesystem::path& fileName) noexcept;