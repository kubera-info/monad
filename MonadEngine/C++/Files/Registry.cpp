// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Registry.h"
// Monad
#include "Tools/Convert.h"
#include "Exceptions/Exceptions.h"

namespace Monad::Files
{
	namespace
	{
		constexpr bool DWORDToBool(DWORD dword) noexcept
		{
			return 0 != dword;
		}

		inline const HKEY RegCreateKeyEx(
			const DWORD options,
			const wchar_t* subKey
		)
		{
			HKEY retSubKey;
			THROW_EXC_IFFALSE(Monad::Exceptions::InvalidCall,
				ERROR_SUCCESS == ::RegCreateKeyEx(
					HKEY_CURRENT_USER,
					subKey,
					0,
					NULL,
					0,
					options,
					NULL,
					&retSubKey,
					NULL),
				L"Reg Create Key Ex");
			return retSubKey;
		}

		LSTATUS RegQueryValueEx(
			HKEY subKey,
			LPCWSTR name,
			LPBYTE data,
			LPDWORD size
		) noexcept
		{
			return ::RegQueryValueEx(
				subKey,
				name,
				nullptr,
				nullptr,
				data,
				size
			);
		}

		void RegSetValueEx(
			HKEY subKey,
			LPCWSTR name,
			DWORD type,
			const BYTE* data,
			DWORD size
		) noexcept
		{
			::RegSetValueEx(
				subKey,
				name,
				0,
				type,
				data,
				size);
		}
	}

#pragma region Accessors
	void Registry::SetString(
		const wchar_t* name,
		const std::string& value
	) noexcept
	{
		/*return ERROR_SUCCESS ==*/ RegSetValueEx(
			m_subKey,
			name,
			REG_SZ,
			reinterpret_cast<const uint8_t*>(Tools::UTF8ToUnicode(value).c_str()),
			static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
	}

	std::string Registry::GetString(
		const wchar_t* name
	) const
	{
		if (DWORD dwLength; ERROR_SUCCESS == RegQueryValueEx(
			m_subKey,
			name,
			nullptr,
			&dwLength)
			)
		{
			if (dwLength <= 1024)
			{
				std::wstring registryStringValue{
					dwLength / sizeof(wchar_t) - 1,
					L'\0',
					Kernel::default_init_allocator<std::wstring::value_type> {}
				};
				THROW_EXC_IFFALSE(Monad::Exceptions::InvalidCall,
					ERROR_SUCCESS == RegQueryValueEx(
						m_subKey,
						name,
						reinterpret_cast<uint8_t*>(registryStringValue.data()),
						&dwLength),
					L"Reg Query Value Ex");
				return Tools::UnicodeToUTF8(registryStringValue);
			}
		}
		return "";
	}

	void Registry::SetDWORD(
		const wchar_t* name,
		DWORD value
	) noexcept
	{
		/*return ERROR_SUCCESS ==*/ RegSetValueEx(
			m_subKey,
			name,
			REG_DWORD,
			reinterpret_cast<const uint8_t*>(&value),
			static_cast<DWORD>(sizeof(DWORD))
		);
	}

	DWORD Registry::GetDWORD(
		const wchar_t* name,
		const DWORD defaultValue
	) const noexcept
	{
		if (DWORD dwLength = sizeof DWORD, dwValue = 0; ERROR_SUCCESS !=
			RegQueryValueEx(
				m_subKey,
				name,
				reinterpret_cast<uint8_t*>(&dwValue),
				&dwLength)
			)
			return defaultValue;
		else
			return dwValue;
	}

	void Registry::SetBool(
		const wchar_t* name,
		bool value
	) noexcept
	{
		SetDWORD(
			name,
			value
		);
	}

	bool Registry::GetBool(
		const wchar_t* name,
		const bool defaultValue
	) const noexcept
	{
		return DWORDToBool(
			GetDWORD(
				name,
				defaultValue)
		);
	}
#pragma endregion

#pragma region Ctors Dtors and tools
	Registry::Registry(
		const DWORD options,
		const std::wstring& subKey
	) :
		m_subKey{
		RegCreateKeyEx(
			options,
			subKey.c_str())
	}
	{}

	Registry::~Registry()
	{
		RegCloseKey(m_subKey);
	}
#pragma endregion
}