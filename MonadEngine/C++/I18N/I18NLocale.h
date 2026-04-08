// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <regex>
#include <string>
#include <vector>

//
// Defines the interface for the I18N system.
// Provides mechanisms for selecting translation sets.
//

namespace Monad::I18N
{
#pragma region API
	/// <summary>
	/// American English, used as the default language when no other match is found.
	/// </summary>
	constexpr const char* DEFAULT_LANGUAGE = "en-US";

	struct SimplifiedLocaleName final
	{
		/// <summary>
		/// Analyzer that resolves the appropriate language based on the selected locale.
		/// </summary>
		std::regex analyzer;
		/// <summary>
		/// When this test passes, `targetLocale` may be applied.
		/// </summary>
		std::string targetLocale;
	};

	/// <summary>
	/// List of transformative locales (regex → locale string).
	/// The order of entries is significant.
	/// There is no need to include "en-US" (DEFAULT_LANGUAGE), as it acts as a catch-all for unmatched locales.
	/// </summary>
	using VectorSimplifiedLocaleNames = std::vector<SimplifiedLocaleName>;

	/// <summary>
	/// Active locale name.
	/// </summary>
	/// <returns>Locale name after parsing and normalization; may differ from the value provided to SetLocaleName().</returns>
	std::string GetLocaleName();

	/// <summary>
	/// Assigns a new locale name.
	/// </summary>
	/// <param name="name">Locale name input to be parsed, validated, and applied.</param>
	void SetLocaleName(
		const std::string& name
	);

	/// <summary>
	/// Parses a locale name.
	/// </summary>
	/// <param name="localeName">The locale name to parse.</param>
	/// <returns>
	/// The computed locale value, based on VectorSimplifiedLocaleNames.
	/// </returns>
	const std::string SimplifyLocaleName(
		const std::string& localeName
	);
}
#pragma endregion

#pragma region User-defined literals
/// <summary>
/// A key-value pair mapping a locale to its corresponding literal string.
/// </summary>
using PairLang = std::pair<std::string, std::wstring>;
/// <summary>
/// Returns a localized string for the specified language.
/// </summary>
/// <param name="countryCode">ISO country code identifying the language.</param>
/// <param name="buffer">Text buffer containing the localized string.</param>
/// <returns>The localized sentence.</returns>
#define MONAD_MAKE_LANG(countryCode, locale) \
	constexpr PairLang operator""_##countryCode(const wchar_t* literal, std::size_t sizeOfString) noexcept\
	{\
		return std::make_pair<std::string, std::wstring>(locale, { literal, sizeOfString });\
	}

	// Example locales build
MONAD_MAKE_LANG(gb, "en-GB");
MONAD_MAKE_LANG(pl, "pl-PL");
MONAD_MAKE_LANG(ru, "ru-RU");
MONAD_MAKE_LANG(us, Monad::I18N::DEFAULT_LANGUAGE);
#pragma endregion
