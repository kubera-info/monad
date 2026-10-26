// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <format>
#include <flat_map>
#include <assert.h>

// Monad
#include "I18NLocale.h"

//
// Header file for the internationalization (I18N) subsystem.
// Provides mechanisms for selecting and managing translation sets.
//
namespace Monad::I18N
{
	template<typename V>
	struct I18NType
	{
		using FlatMapTranslations = std::flat_map<std::wstring, V>;
		FlatMapTranslations m_translations;
		using InitializerListTranslations = std::initializer_list<
			std::pair<std::wstring, V>
		>;
		I18NType() = default;
		I18NType(
			InitializerListTranslations translations
		) :
			m_translations(translations)
		{
			assert(translations.size() == m_translations.size() && "Duplicate translations found.");
		}
		I18NType(
			const V& defaultTranslation
		) :
			I18NType{ { DEFAULT_LANGUAGE, defaultTranslation } }
		{}

		template<typename Self>
		decltype(auto) GetVal(this Self&& self) noexcept
		{
			auto findTransl = self.m_translations.find(g_defaultLocaleManager.GetLocaleName());

			if (self.m_translations.cend() == findTransl)
			{
				findTransl = self.m_translations.find(DEFAULT_LANGUAGE);
				if (self.m_translations.cend() == findTransl)
				{
					findTransl = self.m_translations.begin();
					if (self.m_translations.cend() == findTransl)
						std::unreachable();
				}
			}

			return findTransl->second;
		}
	};

	/// <summary>
	/// Class responsible for storing and managing localized text
	/// in languages defined by the developer.
	/// </summary>
	struct I18NString : I18NType<std::wstring>
	{
		using I18NType::I18NType;
		
		/// <summary>
		/// The size() method is intentionally omitted,
		/// as separate calls would not be atomic and could lead to inconsistent results.
		/// Is also OK to pass this string into Windows API.
		/// </summary>
		/// <returns>LCPWSTR in the current locale</returns>
		const wchar_t* ToLPCWSTR() const noexcept;

		template<typename... A>
		[[nodiscard]]
		std::wstring VFormat(
			A&&... args
		) const
		{
			return std::vformat(
				GetVal(),
				std::make_wformat_args(args...));
		}
	};
}