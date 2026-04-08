// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#define STRICT
#define NODRAWTEXT
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN

// Use the C++ standard templated min/max
#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include <tchar.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

#include "Exceptions/Exceptions.h"