// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "HttpsAPI.h"

// STD
#include <string>

// Monad
#include "AppParameter.h"
#include "Fonts/__MonadFont.h"
#include "resource.h"
#include "System/Parameter.h"

using namespace std::string_literals;

namespace Monad::Internet
{
	using namespace System;

	/// <summary>
	/// Sends a bug report to the remote HTTP endpoint.
	///
	/// The function collects diagnostic data from the application state,
	/// including exception details, environment information and optional
	/// user-provided comment, and forwards it to the server using CurlINet.
	/// </summary>
	/// <returns>
	/// Expected result:
	///  - success → response body
	///  - failure → HTTP status code
	/// </returns>
	CurlINet::ExpectedResponce GetBugReportOnServer()
	{
		CurlINet internetByCurl;

		// Perform HTTP request with collected diagnostic parameters
		return internetByCurl.Get(
			"https://www.kubera.info/api/"s,
			"rep"s,
			{
				g_parameters->QueryParameter(PAR_NAME_WHO_AM_I),
				g_parameters->QueryParameter(PAR_NAME_CLASS),
				{ "Description"s, g_parameters->GetErrorMessageFromHRESULT() },
				g_parameters->QueryParameter(PAR_NAME_EXCEPTION),
				g_parameters->QueryParameter(PAR_NAME_HRESULT),
				g_parameters->QueryParameter(PAR_NAME_PAGE),
				g_parameters->QueryParameter(PAR_NAME_STAGE),
				g_parameters->QueryParameter(PAR_NAME_THREAD_NAME),
				g_parameters->QueryParameter(PAR_NAME_GPU),
				g_parameters->QueryParameter(PAR_NAME_TYPE),
				g_parameters->QueryParameter(PAR_NAME_NATIVE),
				g_parameters->QueryParameter(PAR_NAME_ARM64),
				g_parameters->QueryParameter(PAR_NAME_VERSION)
			}
		);
	}
}