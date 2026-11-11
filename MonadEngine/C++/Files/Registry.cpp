// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Registry.h"

using namespace std;

namespace Monad::Files
{
	namespace
	{
		constexpr bool DWORDToBool(DWORD dword) noexcept
		{
			return !!dword;
		}

		inline const HKEY RegCreateKeyEx(
			const DWORD options,
			const wchar_t* subKey
		)
		{
			HKEY retSubKey;
			if (ERROR_SUCCESS != ::RegCreateKeyEx(
				HKEY_CURRENT_USER,
				subKey,
				0,
				NULL,
				0,
				options,
				NULL,
				&retSubKey,
				NULL)
				)
				throw runtime_error("Reg Create Key Ex");
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
		)
		{
			if (ERROR_SUCCESS != ::RegSetValueEx(
				subKey,
				name,
				0,
				type,
				data,
				size)
				)
				throw runtime_error("Reg Set Value Ex");
		}
	}

#pragma region Accessors
	void Registry::SetString(
		const wchar_t* name,
		const wstring& value
	)
	{
		RegSetValueEx(
			m_subKey,
			name,
			REG_SZ,
			reinterpret_cast<const uint8_t*>(value.c_str()),
			static_cast<DWORD>((value.length() + 1) * sizeof(wchar_t)));
	}

	wstring Registry::GetString(
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
			wstring registryStringValue(dwLength / sizeof(wchar_t) - 1, L'\0');
			if (ERROR_SUCCESS != RegQueryValueEx(
				m_subKey,
				name,
				reinterpret_cast<uint8_t*>(registryStringValue.data()),
				&dwLength)
				)
				throw runtime_error("Reg Query Value Ex");
			return registryStringValue;
		}
		return L"";
	}

	void Registry::SetDWORD(
		const wchar_t* name,
		DWORD value
	)
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
	)
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

#pragma region Ctors Dtors
	Registry::Registry(
		const DWORD options,
		const wstring& subKey
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
