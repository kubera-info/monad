// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <format>
// Monad
#include "Kernel/Del.h"
#include "I18NLocale.h"
#include "Kernel/Templates.h"

//
// Header file for the internationalization (I18N) system.
// Provides mechanisms for selecting and managing translation sets.
//

namespace Monad::I18N
{
	using namespace std::string_literals;

	template<typename V>
	struct I18NType
	{
	protected:
		Kernel::UnorderedMapString<V> m_translations;

	public:
		using InitializerListTranslations = std::initializer_list<
			std::pair<const std::string, V>
		>;

		I18NType(
			InitializerListTranslations translations
		) :
			m_translations(translations)
		{}

		OPER_DEF(I18NType);
		~I18NType() = default;

		const V& GetVal() const
		{
			auto findTransl = m_translations.find(GetLocaleName());

			if (m_translations.cend() == findTransl)
			{
				findTransl = m_translations.find(DEFAULT_LANGUAGE);
				if (m_translations.cend() == findTransl)
				{
					findTransl = m_translations.cbegin();
					if (m_translations.cend() == findTransl)
						std::unreachable();
				}
			}

			return findTransl->second;
		}
	};

	/// <summary>
	/// Class responsible for storing and managing localized text in languages defined by the developer.
	/// </summary>
	struct I18NString : I18NType<std::wstring>
	{
		using I18NType::I18NType;
		OPER_DEF_NO_DEF_CTOR(I18NString);
		~I18NString() = default;
		I18NString(
			const std::wstring& defaultText
		);

		const wchar_t* ToLPCWSTR() const noexcept; // he size() method is intentionally omitted, as separate calls would not be atomic and could lead to inconsistent results.
		std::string ToString() const;

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

	//using I18NFloat = I18NType<float>;
}
