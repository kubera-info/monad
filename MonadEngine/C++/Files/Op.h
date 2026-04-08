// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <securitybaseapi.h>
// STD
#include <filesystem>
// Monad
#include "__MonadFile.h"
#include "System/__MonadSystem.h"
#include "Exceptions/Exceptions.h"
#include "Kernel/BytesVector.h"

namespace Monad::Files
{
	struct Security
	{
		Security();

		SECURITY_DESCRIPTOR m_sd;
		SECURITY_ATTRIBUTES m_sa;
	} extern g_security;

	struct MONAD_CREATEFILE2_EXTENDED_PARAMETERS
	{
		DWORD dwFileAttributesAndFlags;
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr;
	} extern
		g_writeParameters,
		g_safeAccessParameters;

	/// <summary>
	/// A class that can read or write a whole file
	/// </summary>
	struct SmartHandleFile : System::SmartHandle
	{
		using SmartHandle::SmartHandle;

		using OptionalSize = std::optional<uint32_t>;

		SmartHandleFile(
			const std::filesystem::path& path,
			DWORD dwDesiredAccess,
			DWORD dwShareMode,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwCreationDisposition,
			DWORD dwFlagsAndAttributes
		);
		uint32_t FileSize() const;
		OPER_DEL(SmartHandleFile);
		void Read(
			Kernel::VectorBytes& buffer,
			OptionalSize optionalBegin = std::nullopt,
			OptionalSize optionalSize = std::nullopt
		) const;

		template<typename V>
		void Write(
			const V& buffer,
			OptionalSize optionalBegin = std::nullopt,
			OptionalSize optionalSize = std::nullopt
		) const
		{
			OnZipBomb(buffer);
			if (FALSE == ::WriteFile(m_handle,
				reinterpret_cast<const uint8_t*>(buffer.data()) + (optionalBegin ? *optionalBegin : 0ul),
				static_cast<DWORD>(optionalSize ? *optionalSize : buffer.size() * sizeof(V::value_type)),
				nullptr,
				nullptr))
				THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"Write File")
		}
		void SetFilePointerToBegin();
	private:
		template<typename V>
		static void OnZipBomb(
			const V& buffer
		)
		{
			if (buffer.size() * sizeof(V::value_type) > ZIP_BOMB_RANGE_SIZE)
				THROW_EXC(
					Exceptions::FileDamaged,
					HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE),
					L"Pack Bomb #2"
				);
		}
	};

	void RenameFileForBackup(const std::filesystem::path& filename);
}