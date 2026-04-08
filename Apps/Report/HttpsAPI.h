// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

// Monad
#include "Inet/BoxedCurl.h"

namespace Monad::Internet
{
	/// <summary>
	/// Function sends a bug report to the server and receives an issue id
	/// </summary>
	/// <returns>XML with ID or empty string on error</returns>
	CurlINet::ExpectedResponce GetBugReportOnServer();
}