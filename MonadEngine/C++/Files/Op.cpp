// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Op.h"
// STD
#include <string>

namespace fs = std::filesystem;
using namespace std;
using namespace std::string_literals;

namespace Monad::Files
{
	Security::Security()
	{
		// Initialize security descriptor
		THROW_EXC_IFFALSE(Exceptions::InvalidClass, InitializeSecurityDescriptor(&m_sd, SECURITY_DESCRIPTOR_REVISION), L"Security Descriptor");
		THROW_EXC_IFFALSE(Exceptions::InvalidClass, SetSecurityDescriptorDacl(&m_sd, TRUE, NULL, FALSE), L"Set Security Descriptor Dacl");
		m_sa = {
			.nLength = sizeof(SECURITY_ATTRIBUTES),
			.lpSecurityDescriptor = &m_sd,
			.bInheritHandle = TRUE
		}; // Allow handle inheritance
	}
	Security g_security;

#pragma region SmartHandleFile
	MONAD_CREATEFILE2_EXTENDED_PARAMETERS
		g_writeParameters{
			.dwFileAttributesAndFlags =
			FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
	},
		g_safeAccessParameters{
			.dwFileAttributesAndFlags =
			FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
		| FILE_ATTRIBUTE_TEMPORARY
		| FILE_FLAG_DELETE_ON_CLOSE,
			.lpSecurityAttributes = &g_security.m_sa,
	};

	SmartHandleFile::SmartHandleFile(
		const fs::path& path,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes
	) :
		SmartHandle(CreateFileW(
			path.c_str(),
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			nullptr
		))
	{
		if (INVALID_HANDLE_VALUE == m_handle) [[unlikely]]
		{
			const wstring stage =
				L"File not open: "s
				+ path.wstring()
				+ ((0 < (dwDesiredAccess & GENERIC_WRITE)) ? L" mode: write"s : L" mode: read"s
					);
			THROW_EXC(
				Exceptions::IOError,
				HRESULT_FROM_WIN32(GetLastError()),
				stage
			);
		}
	}

	uint32_t SmartHandleFile::FileSize() const
	{
		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(
			m_handle,
			&fileSize
		))
			THROW_EXC(Exceptions::IOError, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), L"File not open #2");
		if (fileSize.HighPart > 0 || fileSize.LowPart > ZIP_BOMB_RANGE_SIZE) [[unlikely]]
			THROW_EXC(Exceptions::IOError, HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE), L"Pack Bomb");
		return fileSize.LowPart;
	}

	void SmartHandleFile::Read(
		Kernel::VectorBytes& buffer,
		OptionalSize optionalBegin,
		OptionalSize optionalSize
	) const
	{
		OnZipBomb(buffer);
		if (FALSE == ::ReadFile(
			m_handle,
			reinterpret_cast<uint8_t*>(buffer.data()) + (optionalBegin ? *optionalBegin : 0ul),
			optionalSize ? *optionalSize : static_cast<DWORD>(buffer.size()),
			nullptr,
			nullptr))
			THROW_EXC(Exceptions::IOError, HRESULT_FROM_WIN32(GetLastError()), L"Read File");
	}
	void SmartHandleFile::SetFilePointerToBegin()
	{
		SetFilePointer(m_handle, 0, nullptr, FILE_BEGIN);
	}
#pragma endregion

	void RenameFileForBackup(
		const fs::path& filename
	)
	{
		const fs::path oldFilename = filename.wstring() + L'~';
		error_code ec;
		fs::remove(oldFilename, ec);
		fs::rename(filename, oldFilename, ec);
	}
}