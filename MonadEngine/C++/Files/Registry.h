// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <string>
// Monad
#include "Globals/__MonadGlobals.h"
#include "Kernel/Del.h"

//
// A wrapper for the Windows Registry API.
//

#define ROOT_REG L"SOFTWARE\\"

namespace Monad::Files
{
	/// <summary>
	/// Windows Registry reader and writer, wrapping the system API.
	/// </summary>
	interface Registry abstract
	{
	protected:
		Registry(
			const DWORD options,
			const std::wstring & subKey = Globals::g_userProfile
		);
		~Registry();

	public:
		void SetString(
			const wchar_t* name,
			const std::string& value
		) noexcept;

		std::string GetString(
			const wchar_t* name
		) const;

		void SetDWORD(
			const wchar_t* name,
			DWORD value
		) noexcept;

		DWORD GetDWORD(
			const wchar_t* name,
			const DWORD defaultValue
		) const noexcept;

		void SetBool(
			const wchar_t* name,
			bool value
		) noexcept;

		bool GetBool(
			const wchar_t* name,
			const bool defaultValue
		) const noexcept;

	private:
		OPER_DEL(Registry);

		const HKEY m_subKey = nullptr;
	};
}
