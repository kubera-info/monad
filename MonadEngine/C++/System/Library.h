// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
// STD
#include <filesystem>
// Monad
#include "Kernel/Del.h"

namespace Monad::System
{
	/// <summary>
	/// Name of the Windows NT native system library.
	/// </summary>
	constexpr wchar_t const* g_Ntdll = L"Ntdll.dll";

	/// <summary>
	/// Base class for loading and accessing dynamic libraries (DLLs).
	/// Provides a thin wrapper over LoadLibrary / GetProcAddress.
	/// </summary>
	interface LibraryGeneric abstract
	{
	protected:
		/// <summary>
		/// Retrieves the address of an exported function from the loaded module.
		/// </summary>
		/// <param name="procedureName">
		/// Name of the exported symbol (ANSI string).
		/// </param>
		/// <returns>
		/// Pointer to the requested function.
		/// The result must be cast to the appropriate function signature.
		/// </returns>
		FARPROC WINAPI GetProcAddress(
			const char* procedureName
		) const noexcept;

		/// <summary>
		/// Loads a dynamic library from a given directory.
		/// </summary>
		/// <param name="folder">
		/// Directory containing the DLL.
		/// </param>
		/// <param name="fileName">
		/// Name of the DLL file.
		/// </param>
		inline LibraryGeneric(
			const std::filesystem::path& folder,
			const wchar_t* fileName
		);

		/// <summary>
		/// Frees the loaded library.
		/// </summary>
		~LibraryGeneric();

		OPER_DEL(LibraryGeneric);

	private:
		/// <summary>
		/// Handle to the loaded module.
		/// </summary>
		const HMODULE m_lib;
	};

	/// <summary>
	/// Loads a DLL from the system directory.
	/// </summary>
	struct SystemLibrary : LibraryGeneric
	{
		/// <summary>
		/// Constructs a system library loader.
		/// </summary>
		/// <param name="fileName">
		/// Name of the system DLL (without path).
		/// </param>
		SystemLibrary(
			const wchar_t* fileName
		);

		OPER_DEL(SystemLibrary);
	};

	/// <summary>
	/// Loads a DLL from the application's binary directory.
	/// </summary>
	struct LocalLibrary : LibraryGeneric
	{
		/// <summary>
		/// Constructs a loader for a local (application) DLL.
		/// </summary>
		/// <param name="fileName">
		/// Name of the DLL located in the application directory.
		/// </param>
		LocalLibrary(
			const wchar_t* fileName
		);

		OPER_DEL(LocalLibrary);
	};
}