// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Globals/__MonadGlobals.h"

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
			const std::wstring& value
		);

		std::wstring GetString(
			const wchar_t* name
		) const;

		void SetDWORD(
			const wchar_t* name,
			DWORD value
		);

		DWORD GetDWORD(
			const wchar_t* name,
			const DWORD defaultValue
		) const noexcept;

		void SetBool(
			const wchar_t* name,
			bool value
		);

		bool GetBool(
			const wchar_t* name,
			const bool defaultValue
		) const noexcept;

	private:
		Registry() = delete;
		Registry(const Registry&) = delete;
		Registry& operator=(const Registry&) = delete;
		Registry(Registry&&) = delete;
		Registry& operator=(Registry&&) = delete;

		const HKEY m_subKey;
	};
}
