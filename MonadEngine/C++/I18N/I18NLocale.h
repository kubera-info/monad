// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>

// STD
#include <regex>
#include <string>
#include <vector>

/// <summary>
/// Defines the interface for the I18N system.
/// Allows selecting the active translation set (locale) at runtime.
/// </summary>
namespace Monad::I18N
{
#pragma region API
	constexpr const wchar_t* DEFAULT_LANGUAGE = L"en_US";

	struct LocaleManager
	{
		LocaleManager();

		/// <summary>
		/// American English, used as the default language
		/// when no other matching is found.
		/// </summary>
		struct SimplifiedLocaleName final
		{
			/// <summary>
			/// Analyzer that resolves the appropriate language
			/// based on the selected locale.
			/// </summary>
			std::wregex analyzer;
			/// <summary>
			/// When this test passes, `targetLocale` may be applied.
			/// </summary>
			std::wstring targetLocale;
		};

		/// <summary>
		/// List of transformative locales (regex → locale string).
		/// The order of entries is significant. There is no need to
		/// The "en_US" (DEFAULT_LANGUAGE) acts as a catch-all.
		/// for unmatched locales.
		/// </summary>
		using VectorSimplifiedLocaleNames = std::vector<SimplifiedLocaleName>;

		/// <summary>
		/// Active locale name.		
		/// </summary>
		/// <returns>Locale name after parsing and normalization;
		/// Locale name after parsing and normalization; may differ from
		/// the value passed to SetLocaleName().
		/// </returns>
		const std::wstring& GetLocaleName() const noexcept;

		/// <summary>
		/// Sets the active locale from a user-supplied name.
		/// </summary>
		/// <param name="name">Locale name in any accepted format (e.g. "pl_PL", "en_GB"); 
		/// will be normalized.
		/// </param>
		void SetLocaleName(
			const std::wstring& name
		);

	private:
		/// <summary>
		/// Resolves a detected locale to a simplified/canonical form using regular expressions.
		/// (e.g. Good: "pl" → "pl_PL", "ru_KZ" → "ru_RU").
		/// (e.g. Wrong: "pl-PL").
		/// </summary>
		const VectorSimplifiedLocaleNames m_simplifiedLocaleNames;
		/// <summary>
		/// Current locale
		/// </summary>
		std::wstring m_applicationLocaleName;
	} extern g_defaultLocaleManager;
}
#pragma endregion

#pragma region User-defined literals
/// <summary>
/// A key-value pair mapping a locale to its corresponding literal string.
/// </summary>
using PairLang = std::pair<std::wstring, std::wstring>;

/// <summary>
/// Returns a localized string for the specified language.
/// New suffix operators.
/// </summary>
/// <param name="countryCode">Language and country code.</param>
/// <returns>The localized sentence.</returns>
#define MONAD_MAKE_LANG(countryCode) \
	constexpr PairLang operator""_##countryCode(\
		const wchar_t* literal,\
		std::size_t sizeOfString) noexcept\
	{\
		return std::make_pair<std::wstring, std::wstring>(\
			{ TEXT(#countryCode), _countof(#countryCode) - 1 },\
			{ literal, sizeOfString });\
	}

/// <summary>
/// New suffix operators
/// </summary>
/// <param name="">Locale</param>
MONAD_MAKE_LANG(en_GB);
MONAD_MAKE_LANG(pl_PL);
MONAD_MAKE_LANG(ru_RU);
MONAD_MAKE_LANG(en_US);
#pragma endregion