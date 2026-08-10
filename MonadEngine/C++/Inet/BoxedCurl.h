// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <expected>
#include <string>

// Monad
#include "Kernel/Del.h"

namespace Monad::Internet
{
	/// <summary>
	/// High-level wrapper around the libcurl C API.
	///
	/// This class provides a safe and minimal abstraction layer over
	/// dynamically loaded libcurl, exposing only what is required
	/// for HTTP-based communication.
	/// </summary>
	struct CurlINet final
	{
		CurlINet();
		~CurlINet();
		OPER_DEL_NO_DEF_CTOR(CurlINet);

		/// <summary>
		/// Result type returned by HTTP requests.
		/// On success: contains response body.
		/// On failure: contains HTTP status code or transport error.
		/// </summary>
		using ExpectedResponce = std::expected<std::string, long>;

		/// <summary>
		/// Represents a single HTTP query parameter.
		/// </summary>
		struct QueryParameter final
		{
			/// <summary>
			/// Parameter name (key).
			/// </summary>
			const std::string c_name;

			/// <summary>
			/// Parameter value (UTF-16, converted internally).
			/// </summary>
			const std::wstring c_value;
		};

		using InitializerListQueryParameters =
			std::initializer_list<QueryParameter>;

		/// <summary>
		/// Executes an HTTP GET request against the given API endpoint.
		/// </summary>
		/// <param name="apiURL">
		/// Base API URL, typically ending with a slash.
		/// </param>
		/// <param name="functionName">
		/// Name of the remote API function to invoke.
		/// </param>
		/// <param name="parameters">
		/// Query parameters appended to the request.
		/// </param>
		/// <returns>
		/// On success: response body.
		/// On failure: HTTP status code or transport error.
		/// </returns>
		ExpectedResponce Get(
			const std::string& apiURL,
			const std::string& functionName,
			InitializerListQueryParameters parameters
		) const;
	};
}