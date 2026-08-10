// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

// Monad
#include "Files/XML.h"

const std::wstring& IsReturnedXMLStatus() noexcept;

/// <summary>
/// Parses XML from www.kubera.info/api/rep and retrieves reference id on success
/// </summary>
struct LoaderXMLStatus final : private Monad::Files::XMLManager
{
	LoaderXMLStatus(
		IStream* const resultXMLStream
	);
};