// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Parameter.h"
// Monad
#include "Fonts/__MonadFont.h"

using namespace std;

namespace Monad::System
{
	// -------------------------------------------------------------------------
	// Parameter names and well-known values
	// -------------------------------------------------------------------------

	const wstring
		FILENAME_REPORT(L"--file"),
		PAR_NAME_CLASS(L"Class"),
		PAR_NAME_EXCEPTION(L"Exception"),
		PAR_NAME_GPU(L"GPU"),
		PAR_NAME_HRESULT(L"HRESULT"),
		PAR_NAME_I18N(L"I18N"),
		PAR_NAME_ICO(L"Ico"),
		PAR_NAME_NATIVE(L"NativeOrWine"),
		PAR_NAME_ARM64(L"Arch"),
		PAR_NAME_PAGE(L"Page"),
		PAR_NAME_PATH(L"Path"),
		PAR_NAME_SALT(L"Salt"),
		PAR_NAME_SHORT_NAME(L"ShortName"),
		PAR_NAME_STAGE(L"Stage"),
		PAR_NAME_THREAD_NAME(L"Thread"),
		PAR_NAME_APP_TITLE(L"Title"),
		PAR_NAME_TYPE(L"Type"),
		PAR_NAME_VERSION(L"Version"),
		PAR_NAME_WHO_AM_I(L"WhoAmI"),
		PAR_VAL_HARDWARE(L"Hardware"),
		PAR_VAL_INFO(L"Info"),
		PAR_VAL_REPORT(L"Report");

	/// <summary>
	/// Obfuscates a string by truncating it and appending an ellipsis.
	/// </summary>
	/// <param name="value">Input string.</param>
	/// <param name="count">Number of characters to keep.</param>
	/// <returns>
	/// Truncated string with ellipsis appended.
	/// </returns>
	wstring Obfuscate(
		const wstring& value,
		size_t count
	) noexcept
	{
		return value.substr(0, count) + Fonts::CHAR_HORIZONTAL_ELLIPSIS;
	}

	/// <summary>
	/// Retrieves a named command-line parameter.
	/// </summary>
	/// <param name="name">
	/// Parameter name (without '=' sign).
	/// </param>
	/// <param name="defaultValue">
	/// Value returned if the parameter is not found.
	/// </param>
	/// <returns>
	/// Decoded parameter value or default value.
	/// </returns>
	wstring GetParameter(
		const wstring& name
	)
	{
		for (size_t i = 1; i < __argc; ++i)
			// This is safe fragment of code,
			// the [] operator is always valid in this sequence of comparisons.
			// It is a C like code.
			if (const wstring parameterIn(__wargv[i]);
				parameterIn.starts_with(name)
				&& L'=' == parameterIn[name.length()]
				&& parameterIn[name.length() + 1])
		{
#if defined _DEBUG
			OutputDebugString(L"---\n");
			OutputDebugString(name.data());
			OutputDebugString(L"---\n");
			OutputDebugString(parameterIn.data() + 1);
			OutputDebugString(L"---\n");
			OutputDebugString(&parameterIn[name.length()]);
			OutputDebugString(L"---\n");
#endif
			wstring pathToXML(parameterIn.substr(name.length() + 1));
			if (pathToXML.starts_with(L'"') && pathToXML.ends_with(L'"'))
				return pathToXML.substr(1, pathToXML.size() - 2);
			else
				return pathToXML;
		}
		return L"";
	}

	namespace
	{
		/// <summary>
		/// Creates a single command-line parameter in the form:
		/// " name=File Path passed with " characters
		/// </summary>
		/// <param name="name">Parameter name.</param>
		/// <param name="value">Unicode value to the report file.</param>
		/// <returns>
		/// Encoded parameter string, or empty string if value is empty.
		/// </returns>
		inline wstring CreateParameter(
			const wstring name,
			const wstring value
		)
		{
			if (value.empty()) [[unlikely]]
				return name;
			else if (value.contains(' ')) [[likely]]
				return L' ' + name + L"=\"" + value + L'"';
			else [[likely]]
				return L' ' + name + L"=" + value;
		}
	}

	/// <summary>
	/// Builds a full command-line string from a list of parameters.
	/// </summary>
	/// <param name="parameters">
	/// List of (name, value) parameter pairs.
	/// </param>
	/// <returns>
	/// Concatenated command-line parameter string.
	/// </returns>
	wstring CreateParameters(
		InitializerListParameters parameters
	)
	{
		wstring retParameters;

		for (const auto& parameter : parameters)
			retParameters += CreateParameter(
				parameter.first,
				parameter.second
			);

		return retParameters;
	}
}