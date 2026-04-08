// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

namespace Monad::System
{
	extern const std::wstring
		FILENAME_REPORT,
		PAR_NAME_CLASS,
		PAR_NAME_EXCEPTION,
		PAR_NAME_GPU,
		PAR_NAME_HRESULT,
		PAR_NAME_I18N,
		PAR_NAME_ICO,
		PAR_NAME_NATIVE,
		PAR_NAME_ARM64,
		PAR_NAME_PAGE,
		PAR_NAME_PATH,
		PAR_NAME_SALT,
		PAR_NAME_SHORT_NAME,
		PAR_NAME_STAGE,
		PAR_NAME_THREAD_NAME,
		PAR_NAME_APP_TITLE,
		PAR_NAME_TYPE,
		PAR_NAME_VERSION,
		PAR_NAME_WHO_AM_I,
		PAR_VAL_HARDWARE,
		PAR_VAL_INFO,
		PAR_VAL_REPORT;

	/// <summary>
	/// Obfuscates a string by truncating it and appending an ellipsis.
	/// Used mainly for privacy or log-safe output.
	/// </summary>
	/// <param name="value">Input string.</param>
	/// <param name="count">Number of visible characters.</param>
	/// <returns>
	/// Obfuscated string with ellipsis appended.
	/// </returns>
	std::wstring Obfuscate(
		const std::wstring& value,
		size_t count = 11
	) noexcept;

	/// <summary>
	/// Retrieves a command-line parameter and decodes it from HEX form.
	/// </summary>
	/// <param name="name">
	/// Name of the parameter (without '=' sign).
	/// </param>
	/// <param name="defaultValue">
	/// Value returned if the parameter is not present.
	/// </param>
	/// <returns>
	/// Decoded parameter value, or default value if not found.
	/// </returns>
	std::wstring GetParameter(
		const std::wstring& name
	);

	using InitializerListParameters =
		std::initializer_list<std::pair<std::wstring, std::wstring>>;

	/// <summary>
	/// Builds a command-line string from a list of parameters.
	/// All values are HEX-encoded.
	/// </summary>
	/// <param name="parameters">
	/// List of parameter name–value pairs.
	/// </param>
	/// <returns>
	/// Encoded parameter string ready for process execution.
	/// </returns>
	std::wstring CreateParameters(
		InitializerListParameters parameters
	);
}