// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <windows.h>
// STD
#include <iomanip>
#include <sstream>

namespace Monad::Tools
{
	/// <summary>
	/// A thin wrapper around std::wostringstream
	/// with enabled exception flags.
	///
	/// Purpose:
	///  - allows using stream-style formatting
	///  - throws on formatting or memory errors
	///  - useful for debug / diagnostic string creation
	/// </summary>
	struct OutputStringStream : std::wostringstream
	{
		OutputStringStream()
		{
			exceptions(failbit | badbit);
		}
	};

	/// <summary>
	/// Converts integral or pointer values into a hexadecimal string.
	/// Format: 0xXXXXXXXX
	///
	/// Intended mainly for:
	/// - diagnostics
	/// - logging
	/// - exception messages
	///
	/// </summary>
	/// <typeparam name="I">Integral or pointer type</typeparam>
	/// <param name="value">Value to be converted</param>
	/// <returns>Hexadecimal string representation</returns>
	template<typename I>
		requires std::is_integral_v<I> || std::is_pointer_v<I>
	std::wstring StringFromAnyULongHex(
		const I value
	)
	{
		OutputStringStream oss;
		oss << L"0x"
			<< std::setfill(L'0')
			<< std::setw(sizeof(I) * 2)
			<< std::hex
			<< value;
		return oss.str();
	}

	/// <summary>
	/// Converts all characters of a string to lowercase (in-place).
	/// Uses towlower → Unicode-safe.
	/// </summary>
	void ToLowerInPlace(
		std::wstring& stringToLower
	) noexcept;

	/// <summary>
	/// Returns a lowercase copy of the input string.
	/// </summary>
	std::wstring ToLower(
		const std::wstring& stringToLower
	);

	/// <summary>
	/// Converts UTF-16 string to UTF-8.
	/// </summary>
	/// <param name="unicodeString">Wide-character string</param>
	/// <returns>UTF-8 encoded std::string</returns>
	std::string UnicodeToUTF8(
		const std::wstring& unicodeString
	);

	/// <summary>
	/// Converts UTF-16 string view to UTF-8.
	/// </summary>
	std::string UnicodeToUTF8View(
		const std::wstring_view unicodeString
	);

	/// <summary>
	/// Converts UTF-8 string to UTF-16.
	/// </summary>
	std::wstring UTF8ToUnicode(
		const std::string& utf8String
	);

	/// <summary>
	/// Converts UTF-8 string view to UTF-16.
	/// </summary>
	std::wstring UTF8ToUnicodeView(
		const std::string_view utf8String
	);

	/// <summary>
	/// Encodes a UTF-16 string into hexadecimal form.
	///
	/// Uses Windows CryptoAPI (CryptBinaryToString).
	/// Intended for:
	/// - safe transport
	/// - command-line arguments
	/// - IPC
	/// </summary>
	std::wstring StringToHex(
		const std::wstring_view stringToEncode
	);

	/// <summary>
	/// Decodes a hexadecimal UTF-16 string back to its original form.
	///
	/// Reverse operation of StringToHex().
	/// </summary>
	std::wstring HexToString(
		const std::wstring_view encodedToString
	);
}

/// <summary>
/// Outputs SYSTEMTIME in human-readable form:
/// YYYY-MM-DD#HH.MM.SS
///
/// Intended for logging and diagnostics.
/// </summary>
std::wostream& operator<<(
	std::wostream& stream,
	const SYSTEMTIME systemTime
	);