// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "I18N/I18N.h"

// =========================================================================
// System-related helpers
// =========================================================================
//
// This header contains small, platform-related utility functions
// that are used globally by the application:
//
//  • application version formatting
//  • architecture detection
//  • runtime environment detection (Windows / Wine)
//
// No state is stored here – all functions are stateless and safe to call
// from anywhere in the application.
// =========================================================================

namespace Monad
{
	namespace I18N
	{
		/// <summary>
		/// Localized label used to display application version information.
		/// 
		/// The formatting placeholder `{}` is replaced by:
		///     "<version> <architecture>"
		/// e.g.:
		///     "1.2.0 X64"
		///     "1.2.0 ARM64"
		/// </summary>
		const I18NString APP_VER_LABEL{
			L"Wersja aplikacji: {}"_pl,
			L"Версия приложения: {}"_ru,
			L"The application's version: {}"_us
		};
	}

	namespace System
	{
		/// <summary>
		/// Returns the application version combined with CPU architecture.
		/// 
		/// Example:
		///     "1.2.0 X64"
		///     "1.2.0 ARM64"
		/// 
		/// The version is extracted from the executable's version resource.
		/// </summary>
		/// <returns>
		/// Human-readable version string with architecture suffix.
		/// </returns>
		std::wstring GetAppVersionAndArch() noexcept;

		/// <summary>
		/// Detects whether the application is running under Wine.
		/// 
		/// This is done by probing NTDLL for Wine-specific exports.
		/// </summary>
		/// <returns>
		/// true  - application runs under Wine  
		/// false - native Windows environment
		/// </returns>
		bool IsOSWine();

		/// <summary>
		/// Detects whether the application is running on ARM64 architecture.
		/// 
		/// Internally uses IsWow64Process2 when available.
		/// </summary>
		/// <returns>
		/// true  - ARM64
		/// false - x64 or other
		/// </returns>
		bool GetArchitecture();
	}
}