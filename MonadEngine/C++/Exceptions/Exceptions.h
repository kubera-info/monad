// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
#include <bcrypt.h>
// Monad
#include "__MonadException.h"
#include "Kernel/Del.h"
#include "Catch.h"
#include "System/Library.h"

namespace Monad::Exceptions
{
	inline constexpr const wchar_t* g_childEvent = L"Global\\MonadaChildReadyEvent";

	/// <summary>
	/// Function invoked in the context of a hardware exception.
	/// </summary>
	/// <param name="code">Exception code</param>
	/// <param name="ep">Contains an exception record providing a machine-independent description of the exception, and a context record detailing the machine-dependent processor state at the time of the exception.</param>
	/// <returns></returns>
	int32_t filter(
		[[maybe_unused]] uint32_t code,
		[[maybe_unused]] struct _EXCEPTION_POINTERS* exceptionPointers
	) noexcept;

	HRESULT NtStatusToHresult(
		NTSTATUS status
	) noexcept;
	/// <summary>
	/// Determines if the current context involves a runtime or hardware exception.
	/// </summary>
	/// <returns>Returns true if the application is in the process of closing.</returns>
	bool InAnyCloseReason() noexcept;
	/// <summary>
	/// Wellnes of the application.
	/// </summary>
	/// <returns>True if no error.</returns>
	bool IsValid() noexcept;
	/// <summary>
	/// Checks whether the application is in the process of clean exiting.
	/// </summary>
	/// <returns>True if WM_CLOSE has been sent to the application window</returns>
	bool IsWM_CLOSE() noexcept;
	/// <summary>
	/// Records that the WM_CLOSE event occurred, initiating the application exit sequence.
	/// </summary>
	void RegisterWM_CLOSE() noexcept;
	/// <summary>
	/// Generic Monad exception class; does not have a virtual destructor.
	/// </summary>
	struct ExceptionGeneric
	{
		ExceptionGeneric(
			const std::exception& stdException
		);
		~ExceptionGeneric() = default;
		OPER_DEF_NO_DEF_CTOR(ExceptionGeneric);

	protected:
		ExceptionGeneric(
			const HRESULT hr,
			const std::wstring& stage,
			const std::wstring& message,
			const std::string& exceptionClass
		) noexcept;
	};

#define CREATE_MONAD_EXCEPTION_CLASS( CLASS_NAME, DEFAULT_MESSAGE ) \
		struct CLASS_NAME : ExceptionGeneric \
			{ \
			CLASS_NAME(const HRESULT hr, const std::wstring& stage) noexcept : \
			ExceptionGeneric(\
			hr, \
			stage, \
			DEFAULT_MESSAGE.GetVal(), \
			typeid(*this).name()\
			) \
			{} \
		}

#pragma region Macros for throwing exceptions
#define THROW_EXC(MONAD_EXC_CLASS, MONAD_EXC_HRES, MONAD_EXC_STAGE)\
		{\
			throw MONAD_EXC_CLASS(MONAD_EXC_HRES, MONAD_EXC_STAGE);\
		}
#define THROW_EXC_IFNULL(MONAD_EXC_CLASS, X, MONAD_EXC_STAGE)\
		{ if (nullptr == X) THROW_EXC(MONAD_EXC_CLASS, E_FAIL, MONAD_EXC_STAGE) }
#define THROW_EXC_IFFALSE(MONAD_EXC_CLASS, X, MONAD_EXC_STAGE)\
		{ if (!(X)) THROW_EXC(MONAD_EXC_CLASS, E_FAIL, MONAD_EXC_STAGE) }
#define THROW_EXC_ONEND(END, IT, MONAD_EXC_STAGE)\
		{ if ((END).cend() == IT) THROW_EXC(Monad::Exceptions::OutOfRange, E_FAIL, MONAD_EXC_STAGE) }
#define THROW_EXC_GETLASTERROR( MONAD_EXC_CLASS, MONAD_EXC_STAGE)\
		{ THROW_EXC(MONAD_EXC_CLASS, HRESULT_FROM_WIN32(GetLastError()), MONAD_EXC_STAGE) }
#define THROW_EXC_ONERROR( MONAD_EXC_CLASS, MONAD_ERROR_CODE, MONAD_EXC_STAGE)\
		{ if (ERROR_SUCCESS != MONAD_ERROR_CODE) THROW_EXC(MONAD_EXC_CLASS, HRESULT_FROM_WIN32(MONAD_ERROR_CODE), MONAD_EXC_STAGE) }
#define THROW_EXC_IFFAILED( MONAD_EXC_CLASS, MONAD_EXC_HRES, MONAD_EXC_STAGE)\
		{ if (FAILED(MONAD_EXC_HRES)) THROW_EXC(MONAD_EXC_CLASS, MONAD_EXC_HRES, MONAD_EXC_STAGE) }
#define THROW_EXC_IFFAILED_DB( MONAD_EXC_HRES, MONAD_EXC_STAGE)\
		{ THROW_EXC_IFFAILED(Monad::Exceptions::InvalidXML, MONAD_EXC_HRES, L"DB " MONAD_EXC_STAGE) }
#define THROW_EXC_IFFAILED_RO(X) THROW_EXC_IFFAILED(Exceptions::NotInitialized, X, L"RO Init")
#define THROW_EXC_NTERROR(X, MONAD_EXC_STAGE )\
		{ if (!BCRYPT_SUCCESS(X)) THROW_EXC(Exceptions::InvalidClass, Exceptions::NtStatusToHresult(X), L"Dread " MONAD_EXC_STAGE) }
#define THROW_EXC_AUDIO( MONAD_EXC_HRES, MONAD_EXC_STAGE)\
		{ if (FAILED(MONAD_EXC_HRES)) THROW_EXC(Monad::Exceptions::AudioFailed, MONAD_EXC_HRES, L"Audio " MONAD_EXC_STAGE) }
#define THROW_EXC_D3D( MONAD_EXC_HRES, MONAD_EXC_STAGE)\
		{ if (FAILED(MONAD_EXC_HRES)) THROW_EXC(Monad::Exceptions::ThreeDFailed, MONAD_EXC_HRES, L"3D " MONAD_EXC_STAGE) }
	CREATE_MONAD_EXCEPTION_CLASS(NullValue, I18N::Errors::NULL_VALUE);
	CREATE_MONAD_EXCEPTION_CLASS(InvalidCall, I18N::Errors::INVALID_CALL);

	// Out of memory
	CREATE_MONAD_EXCEPTION_CLASS(NotInitialized, I18N::Errors::NOT_INITIALIZED);

	// Out of range
	CREATE_MONAD_EXCEPTION_CLASS(OutOfRange, I18N::Errors::OUT_OF_RANGE);

	// File IO_ERROR Error
	CREATE_MONAD_EXCEPTION_CLASS(IOError, I18N::Errors::IO_ERROR);
	CREATE_MONAD_EXCEPTION_CLASS(InvalidXML, I18N::Errors::INVALID_XML);

	CREATE_MONAD_EXCEPTION_CLASS(InvalidClass, I18N::Errors::INVALID_CLASS);

	// Runtime
	CREATE_MONAD_EXCEPTION_CLASS(RoInitializationFailed, I18N::Errors::COM_INITIALIZATION_FAILED);

	// File IO_ERROR Error, engine's level
	CREATE_MONAD_EXCEPTION_CLASS(FileDamaged, I18N::Errors::FILE_DAMAGED);
	CREATE_MONAD_EXCEPTION_CLASS(FileNotFound, I18N::Errors::FILE_NOT_FOUND);

	// Database-repo
	CREATE_MONAD_EXCEPTION_CLASS(ClassAlreadyExists, I18N::Errors::CLASS_ALREADY_EXISTS);
	CREATE_MONAD_EXCEPTION_CLASS(NoDataFound, I18N::Errors::NO_DATA_FOUND);

	// DirectX
	CREATE_MONAD_EXCEPTION_CLASS(AudioFailed, I18N::Errors::AUDIO_FAILED);
	CREATE_MONAD_EXCEPTION_CLASS(ThreeDFailed, I18N::Errors::THREE_D_FAILED);
	CREATE_MONAD_EXCEPTION_CLASS(CapsFailed, I18N::Errors::CAPS_FAILED);
#pragma endregion

	[[noreturn]]
	/// <summary>
	/// Engine-specific termination function analogous to std::terminate.
	/// </summary>
	void ReportAndTerminate() noexcept;

#if defined _DEBUG
	struct SystemLibraryDXGI : private System::SystemLibrary
	{
		SystemLibraryDXGI() :
			SystemLibrary{ L"dxgidebug.dll" }
		{}

		using LPDXGIGETDEBUGINTERFACE = HRESULT(WINAPI*)(REFIID, void**);
		const LPDXGIGETDEBUGINTERFACE c_fnDXGIGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(GetProcAddress("DXGIGetDebugInterface"));
	};
#endif
}
