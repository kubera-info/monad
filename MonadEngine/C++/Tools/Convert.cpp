// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Convert.h"
// STD
#include <assert.h>
#include <algorithm>
#include "Exceptions/Exceptions.h"

using namespace std;

namespace Monad::Tools
{
	/// <summary>
	/// Converts all characters in a string to lowercase (in-place).
	/// Safe for Unicode (uses towlower).
	/// </summary>
	void ToLowerInPlace(
		wstring& stringToLower
	) noexcept
	{
		for (auto& chr : stringToLower)
			chr = towlower(chr);
	}

	/// <summary>
	/// Returns a lowercase copy of the input string.
	/// </summary>
	wstring ToLower(
		const wstring& stringToLower
	)
	{
		wstring lowerCaseBuffer;
		ranges::transform(
			stringToLower,
			back_inserter(lowerCaseBuffer),
			towlower
		);
		return lowerCaseBuffer;
	}

	namespace
	{
		/// <summary>
		/// Converts a UTF-16 buffer to UTF-8.
		/// 
		/// Defensive style:
		/// - nullptr check on the left side prevents accidental assignment
		/// - zero-length input is safely ignored
		/// </summary>
		string UnicodeToUTF8(
			const wchar_t* unicodeString,
			const size_t length
		)
		{
			string utf8Buffer;

			// Defensive check:
			// nullptr on the left prevents accidental assignment
			assert(unicodeString);
			__assume(nullptr != unicodeString);
			if (0 < length)
			{
				utf8Buffer.resize(
					WideCharToMultiByte(
						CP_UTF8,
						0,
						unicodeString,
						static_cast<DWORD>(length),
						nullptr,
						0,
						nullptr,
						nullptr)
				);

				if (!WideCharToMultiByte(
					CP_UTF8,
					0,
					unicodeString,
					static_cast<DWORD>(length),
					reinterpret_cast<char*>(utf8Buffer.data()),
					static_cast<DWORD>(utf8Buffer.size()),
					nullptr,
					nullptr
				))
					THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"UnicodeToUTF8");
			}
			return utf8Buffer;
		}
	}

	string UnicodeToUTF8(
		const wstring& unicodeString
	)
	{
		return UnicodeToUTF8(
			unicodeString.data(),
			unicodeString.length()
		);
	}

	string UnicodeToUTF8View(
		const wstring_view unicodeString
	)
	{
		return UnicodeToUTF8(
			unicodeString.data(),
			unicodeString.length()
		);
	}

	namespace
	{
		/// <summary>
		/// Converts UTF-8 buffer to UTF-16 string.
		/// </summary>
		wstring UTF8ToUnicode(
			const char* utf8String,
			const size_t length
		)
		{
			wstring unicodeBuffer;

			// Safe nullptr-first comparison
			assert(utf8String);
			__assume(nullptr != utf8String);
			if (0 < length)
			{
				unicodeBuffer.resize(
					MultiByteToWideChar(
						CP_UTF8,
						0,
						utf8String,
						static_cast<DWORD>(length),
						nullptr,
						0)
				);

				if (!MultiByteToWideChar(
					CP_UTF8,
					0,
					utf8String,
					static_cast<DWORD>(length),
					unicodeBuffer.data(),
					static_cast<DWORD>(unicodeBuffer.length())
				))
					THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"MultiByteToWideChar");
			}
			return unicodeBuffer;
		}
	}

	std::wstring UTF8ToUnicode(
		const std::string& utf8String
	)
	{
		return UTF8ToUnicode(
			utf8String.data(),
			utf8String.size()
		);
	}

	std::wstring UTF8ToUnicodeView(
		const std::string_view utf8String
	)
	{
		return UTF8ToUnicode(
			utf8String.data(),
			utf8String.size()
		);
	}
	/*
	/// <summary>
	/// Converts UTF-16 string into HEX representation.
	/// Used for safe transport via command line or IPC.
	/// </summary>
	wstring StringToHex(
		const wstring_view stringToEncode
	)
	{
		if (stringToEncode.empty())
			return L"";

		DWORD newLen{};
		if (!CryptBinaryToString(
			reinterpret_cast<const uint8_t*>(stringToEncode.data()),
			static_cast<DWORD>(stringToEncode.length() * sizeof(wchar_t)),
			CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF,
			nullptr,
			&newLen))
			throw runtime_error("Error: StringToHex-analyze");

		wstring hexBuffer(
			newLen - 1,
			L'\0',
			Kernel::default_init_allocator<std::wstring::value_type>{}
		);

		if (!CryptBinaryToString(
			reinterpret_cast<const uint8_t*>(stringToEncode.data()),
			static_cast<DWORD>(stringToEncode.length() * sizeof(wchar_t)),
			CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF,
			hexBuffer.data(),
			&newLen))
			throw runtime_error("Error: StringToHex-encode");

		return hexBuffer;
	}

	/// <summary>
	/// Decodes HEX string back into UTF-16.
	/// </summary>
	wstring HexToString(
		const wstring_view encodedToString
	)
	{
		if (encodedToString.empty())
			return L"";

		DWORD newLen{};
		if (!CryptStringToBinary(
			encodedToString.data(),
			0,
			CRYPT_STRING_HEXRAW,
			nullptr,
			&newLen,
			nullptr,
			nullptr))
			throw runtime_error("Error: HexToString-analyze");

		wstring stringBuffer(
			newLen >> 1,
			L'\0',
			Kernel::default_init_allocator<std::wstring::value_type>{}
		);

		if (!CryptStringToBinary(
			encodedToString.data(),
			0,
			CRYPT_STRING_HEXRAW,
			reinterpret_cast<uint8_t*>(stringBuffer.data()),
			&newLen,
			nullptr,
			nullptr))
			throw runtime_error("Error: HexToString-decode");

		return stringBuffer;
	}
	*/
}

/// <summary>
/// Streams SYSTEMTIME in readable ISO-like format:
/// YYYY-MM-DD#HH.MM.SS
/// </summary>
wostream& operator<<(wostream& stream, const SYSTEMTIME systemTime)
{
	stream << systemTime.wYear
		<< L'-'
		<< setw(2) << setfill(L'0') << systemTime.wMonth
		<< setw(0) << L'-'
		<< setw(2) << systemTime.wDay
		<< setw(0) << L'_'
		<< setw(2) << systemTime.wHour
		<< setw(0) << L'.'
		<< setw(2) << systemTime.wMinute
		<< setw(0) << L'.'
		<< setw(2) << systemTime.wSecond;

	return stream;
}