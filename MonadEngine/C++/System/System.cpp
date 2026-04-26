// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "System.h"
// Platform
#pragma comment(lib, "version.lib")
// Monad
#include "Exceptions/Exceptions.h"
#include "Files/Folder.h"
#include "Kernel/BytesVector.h"

namespace Monad::System
{
	/// <summary>
	/// Helper structure used to retrieve application version information
	/// from the executable's VERSIONINFO resource.
	/// </summary>
	struct AppVersion final
	{
		/// <summary>
		/// Reads file version information from the running module.
		/// </summary>
		AppVersion()
		{
			DWORD dummyDW{};
			const auto modulePath = Files::GetModulePath();
			const auto fviSize = GetFileVersionInfoSize(modulePath.c_str(), &dummyDW);

			Kernel::VectorBytes fileVersionInfo(fviSize);

			if (FALSE == GetFileVersionInfo(
				modulePath.c_str(),
				0,
				fviSize,
				fileVersionInfo.data()))
				THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"Get File Version Info");

			uint32_t outLen(0);
			VS_FIXEDFILEINFO* lpFfi;

			if (FALSE == VerQueryValue(
				fileVersionInfo.data(),
				L"\\",
				reinterpret_cast<LPVOID*>(&lpFfi),
				&outLen))
				THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"Ver Query Value");

			m_appVersion =
				std::to_wstring(HIWORD(lpFfi->dwFileVersionMS)) + L'.' +
				std::to_wstring(LOWORD(lpFfi->dwFileVersionMS)) + L'.' +
				std::to_wstring(HIWORD(lpFfi->dwFileVersionLS));
		}

		/// <summary>
		/// Returns the architecture of the currently built binary.
		/// </summary>
		/// <returns>
		/// "ARM64" or "X64"
		/// </returns>
		static std::wstring GetAppArch() noexcept
		{
#if defined _M_ARM64
			return L"ARM64";
#elif defined _M_X64
			return L"X64";
#else
#	error Unknown processor architecture.
#endif
		}

		/// <summary>
		/// Returns formatted application version with architecture suffix.
		/// </summary>
		/// <returns>
		/// Version string in format: "major.minor.build ARCH"
		/// </returns>
		std::wstring GetAppVersionAndArch() const noexcept
		{
#if defined _DEBUG
			return m_appVersion + L" (Debug) " + GetAppArch();
#else
			return m_appVersion + L' ' + GetAppArch();
#endif
		}

	private:
		/// <summary>
		/// Cached application version string.
		/// </summary>
		std::wstring m_appVersion;
	};

	/// <summary>
	/// Returns the application version combined with CPU architecture.
	/// </summary>
	/// <returns>
	/// Human-readable version string.
	/// </returns>
	std::wstring GetAppVersionAndArch() noexcept
	{
		return AppVersion{}.GetAppVersionAndArch();
	}

	/// <summary>
	/// Detects whether the application is running under Wine.
	/// </summary>
	/// <returns>
	/// True if Wine environment is detected, otherwise false.
	/// </returns>
	bool IsOSWine()
	{
		struct SystemLibraryNtdll : private SystemLibrary
		{
			SystemLibraryNtdll() : SystemLibrary{ g_Ntdll } {}

			/// <summary>
			/// Checks for Wine-specific exported symbol.
			/// </summary>
			bool IsOSWine() const noexcept
			{
				return nullptr != GetProcAddress("wine_get_host_version");
			}
		};

		return SystemLibraryNtdll{}.IsOSWine();
	}

	/// <summary>
	/// Determines whether the host system architecture is ARM64.
	/// </summary>
	/// <returns>
	/// True if the native system architecture is ARM64.
	/// </returns>
	bool GetArchitecture()
	{
		USHORT
			processMachine = 0,
			nativeMachine = 0;

		// Use IsWow64Process2 dynamically for compatibility with older systems
		using LPFN_ISWOW64PROCESS2 = BOOL(WINAPI*)(HANDLE, USHORT*, USHORT*);

		if (auto fnIsWow64Process2 =
			reinterpret_cast<LPFN_ISWOW64PROCESS2>(
				GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process2")); fnIsWow64Process2)
			if (fnIsWow64Process2(GetCurrentProcess(), &processMachine, &nativeMachine))
				return (nativeMachine == IMAGE_FILE_MACHINE_ARM64);

		return false;
	}
}