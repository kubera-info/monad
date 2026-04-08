// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Exceptions.h"
// Platform
#if defined _DEBUG
#	pragma comment(lib, "comsuppwd.lib")
#else
#	pragma comment(lib, "comsuppw.lib")
#endif
#include <comdef.h>
// STD
#include <atomic>
#include <mutex>
#include <xcall_once.h>
// Monad
#include "System/__MonadSystem.h"
#include "System/Console.h"
#include "Globals/__MonadGlobals.h"
#include "GUI/__MonadGUI.h"
#include "Files/Temporary.h"
#include "Files/XML.h"
#include "Files/SaveUTF8.h"
#include "Pages/__MonadPage.h"
#include "System/Execute.h"
#include "System/System.h"
#include "System/Exit.h"
#include "System/Library.h"
#include "System/Parameter.h"
#include "Tools/Convert.h"
#include "Files/Folder.h"

using namespace std;
namespace fs = std::filesystem;

#if MONAD_HARDWARE_EXCEPTIONS == 1
#	pragma message ("Supported hardware exceptions.")
#endif

namespace Monad
{
	using namespace Files;
	using namespace Globals;
	using namespace System;
	using namespace Tools;

	namespace Exceptions
	{
		namespace
		{
			enum CLOSE_REASON_STATES : size_t
			{
				CLOSE_REASON_STATE_OK,
				CLOSE_REASON_STATE_WM_CLOSE_CALLED,
				CLOSE_REASON_STATE_CPP_EXCEPTION,
				CLOSE_REASON_STATE_HARDWARE_EXCEPTION
			};
			volatile CLOSE_REASON_STATES g_closeReason = CLOSE_REASON_STATE_OK;

			bool SetReason(
				CLOSE_REASON_STATES closeReason
			) noexcept
			{
				bool result = false;
				static once_flag C;
				call_once(C, [&] noexcept
					{
						result = true;
						g_closeReason = closeReason;
					}
				);
				return result;
			}

			[[noreturn]]
			inline void Fatal() noexcept
			{
#if defined _DEBUG
				OutputDebugString(L"Fatal, stack of exceptions\n");
#else
				ExitProcess(EXIT_FAILURE);
#endif
			}

			void MsgBox(
				[[maybe_unused]] const wstring& exceptionClass,
				[[maybe_unused]] const wstring& report,
				[[maybe_unused]] const HRESULT hr,
				[[maybe_unused]] const wstring& stage,
				[[maybe_unused]] const CLOSE_REASON_STATES closeReason
			)
			{
				if (!SetReason(closeReason))
					return;
				MndOutputDebugString(L"---\nException has been created\n");
				MndOutputDebugString(stage);
				MndOutputDebugString(StringFromAnyULongHex(hr));
#if defined _DEBUG
				MndOutputDebugString(_com_error(hr).ErrorMessage());
				__debugbreak();
#endif

				switch (Globals::g_exceptionReportingMethod)
				{
					using enum EXCEPTION_PRESENTATION;
				case REPORT_EXE:
				{
					{
						/// <summary>
						/// Low quality as a safe mode settings
						/// </summary>
						/*struct RegistryWriteSafeMode : Registry
						{
							inline RegistryWriteSafeMode() noexcept :
								Registry{ KEY_WRITE }
							{
								SetDWORD(L"Quality", 0ul);
							}
						} safeModeRegistry;*/
					}

					wstring errorType;
					if (CLOSE_REASON_STATE_HARDWARE_EXCEPTION == closeReason)
						errorType = PAR_VAL_HARDWARE;
					else
						if (exceptionClass.ends_with(L"CapsFailed"s))
							errorType = PAR_VAL_INFO;
						else
							errorType = PAR_VAL_REPORT;
					//const HRESULT hr = DXGI_ERROR_DEVICE_REMOVED == hr ? g_d3d12Core->m_device->GetDeviceRemovedReason() : hr;

					const fs::path pathToReportXML = fs::temp_directory_path() / (Files::GlobalFileNameSuffix().wstring() + L".$$$"s);

					PWSTR threadName = nullptr;
					wstring strThreadName(L"Monad Anonymous"s);
					if (SUCCEEDED(GetThreadDescription(GetCurrentThread(), &threadName)))
					{
						struct ScopedString
						{
							ScopedString(PWSTR& name) :
								m_threadName(name)
							{}

							~ScopedString()
							{
								LocalFree(m_threadName);
								m_threadName = nullptr;
							}

						private:
							PWSTR& m_threadName;
						} freeMe(threadName);
						if (*threadName)
							strThreadName = threadName;
					}
					{
						const auto xmlFileImage = XMLManager{ {
									{ PAR_NAME_CLASS, exceptionClass },
									{ PAR_NAME_EXCEPTION, report },
									{ PAR_NAME_HRESULT, StringFromAnyULongHex(hr) },
									{ PAR_NAME_STAGE, stage },
									{ PAR_NAME_PAGE, UTF8ToUnicode(Pages::g_currentPage) },
									{ PAR_NAME_TYPE, errorType },
									{ PAR_NAME_THREAD_NAME, strThreadName },
									{ PAR_NAME_I18N, UTF8ToUnicode(I18N::GetLocaleName()) },
									{ PAR_NAME_GPU, g_adapterDesc },
									{ PAR_NAME_APP_TITLE, GUI::RemoveEmojiIcon(g_fullAppName) },
									{ PAR_NAME_SALT, UTF8ToUnicode(g_sha256Salt) },
									{ PAR_NAME_SHORT_NAME, GUI::RemoveEmojiIcon(Globals::g_shortAppName) },
									{ PAR_NAME_PATH, GetModulePath() },
									{ PAR_NAME_WHO_AM_I, g_whoAmI },
									{ PAR_NAME_VERSION, GetAppVersionAndArch() },
									{ PAR_NAME_NATIVE, IsOSWine() ? L"WineHQ"s : L"Native Windows"s },
									{ PAR_NAME_ARM64, GetArchitecture() ? L"ARM64"s : L"X64"s },
									{ PAR_NAME_ICO, to_wstring(g_sourceIcon) }
							}, XML_NO_ELEMENTS }.BuildXML();
						SmartHandle fileHandle = SaveStringAsWithDACL(
							pathToReportXML,
							xmlFileImage);

						System::SmartHandle event(CreateEvent(NULL, TRUE, FALSE, g_childEvent));

#	if defined _DEBUG
						OutputDebugStringA(("\n" + xmlFileImage + "\n").c_str());
						OutputDebugString(L"\n");
						OutputDebugString((L"\n" + pathToReportXML.wstring() + L"\n").c_str());
						OutputDebugString(L"\n");
#	endif
						// Calls the report.exe tool. It can send bug descritpion to the web server.
						ExecuteApp(
							GetFolderBin() / L"Report.exe"sv,
							{
								{
									FILENAME_REPORT,
									to_wstring((unsigned long long) fileHandle.operator const HANDLE())
								}
							}
						);
						/*DWORD result = */ WaitForSingleObject(event, 5000);
					}
				}
				break;
				case MESSAGE_BOX:
				{
					constexpr wstring_view horizontal(L"\n--------------------\n");
					wstring excTitle = I18N::Errors::EXCEPTION_TITLE.GetVal();
					const wstring errStr = excTitle.
						append(horizontal).
						append(report).
						append(L".\n").
						append(horizontal).
						append(L"Class: ").
						append(exceptionClass).
						append(horizontal).
						append(L"Operation: ").
						append(stage).
						append(horizontal).
						append(L"Code: ").
						append(StringFromAnyULongHex(hr)).
						append(horizontal).
						append(L"Message: ").
						append(_com_error(hr).ErrorMessage());
					MessageBox(0, errStr.c_str(), Globals::g_fullAppName.GetVal().c_str(), MB_ICONERROR | MB_TASKMODAL | MB_ICONERROR);
				}
				break;
				case REPORT_TO_CONSOLE:
				{
					constexpr wstring_view horizontal(L"\n\t--------------------\n\t");
					wstring excTitle = I18N::Errors::EXCEPTION_TITLE.GetVal();
					const wstring errStr = L'\t' + excTitle.
						append(horizontal).
						append(report).
						append(L".\n").
						append(horizontal).
						append(L"\tClass: ").
						append(exceptionClass).
						append(horizontal).
						append(L"Operation: ").
						append(stage).
						append(horizontal).
						append(L"Code: ").
						append(StringFromAnyULongHex(hr)).
						append(horizontal).
						append(L"Message: ").
						append(_com_error(hr).ErrorMessage());
					OutputDebugString(L"\nError: ");
					OutputDebugString(errStr.c_str());
					OutputDebugString(__wargv[0]);
					OutputDebugString(L"\n");
				}
				break;
				default:
					unreachable();
				}
				Shutdown(EXIT_FAILURE);
			}
		}

		int32_t filter(
			[[maybe_unused]] uint32_t code,
			[[maybe_unused]] struct _EXCEPTION_POINTERS* exceptionPointers
		) noexcept
		{
#	if MONAD_HARDWARE_EXCEPTIONS == 1
			HRESULT hr = NtStatusToHresult(code);
			MsgBox(PAR_VAL_HARDWARE,
				I18N::Errors::HARDWARE_EXCEPTION.GetVal(),
				hr,
				StringFromAnyULongHex(exceptionPointers->ExceptionRecord->ExceptionAddress),
				CLOSE_REASON_STATE_HARDWARE_EXCEPTION);
			Fatal();
#	else
			return EXCEPTION_CONTINUE_SEARCH;
#	endif
		}

		HRESULT NtStatusToHresult(
			NTSTATUS status
		) noexcept
		{
			struct SystemLibraryNtdll : private SystemLibrary
			{
				SystemLibraryNtdll() : SystemLibrary{ g_Ntdll } {}

				using LPRtlNtStatusToDosError = DWORD(WINAPI*)(NTSTATUS);
				const LPRtlNtStatusToDosError c_fnRtlNtStatusToDosError = reinterpret_cast<LPRtlNtStatusToDosError>(GetProcAddress("RtlNtStatusToDosError"));
			};
			return HRESULT_FROM_WIN32(SystemLibraryNtdll{}.c_fnRtlNtStatusToDosError(status));
		}

		bool IsValid() noexcept
		{
			return g_closeReason < CLOSE_REASON_STATE_CPP_EXCEPTION;
		}

		bool IsWM_CLOSE() noexcept
		{
			return CLOSE_REASON_STATE_WM_CLOSE_CALLED == g_closeReason;
		}

		bool InAnyCloseReason() noexcept
		{
			return CLOSE_REASON_STATE_OK != g_closeReason;
		}

		void RegisterWM_CLOSE() noexcept
		{
			SetReason(CLOSE_REASON_STATE_WM_CLOSE_CALLED);
		}

		[[noreturn]]
		void ReportAndTerminate() noexcept
		{
			MsgBox(
				L"terminate",
				I18N::Errors::EXCEPTION_TITLE.GetVal(),
				E_FAIL,
				L"Terminate",
				CLOSE_REASON_STATE_CPP_EXCEPTION
			);
			Fatal();
		}

		ExceptionGeneric::ExceptionGeneric(
			const HRESULT hr,
			const wstring& stage,
			const wstring& message,
			const string& exceptionClass
		) noexcept
		{
			MndOutputDebugString(L"---\n");
			MsgBox(
				UTF8ToUnicode(exceptionClass),
				message,
				hr,
				stage,
				CLOSE_REASON_STATE_CPP_EXCEPTION
			);
		}

		ExceptionGeneric::ExceptionGeneric(
			const exception& stdException
		) :
			ExceptionGeneric{
				E_FAIL,
				L"Std Exception",
				UTF8ToUnicode(stdException.what()),
				typeid(stdException).name()
		}
		{}
	}
}