// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Trace.h"

#if defined(MONAD_TEST)
// Platform
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"

// Monad
#	include "Files/Folder.h"
#	include "Kernel/Patterns.h"
#	include "Time/__MonadTime.h"
#	include "Globals/__MonadGlobals.h"
#	include "Files/Op.h"

using namespace std;

namespace Monad
{
	using namespace Kernel;
	using namespace Threads;

	namespace Exceptions
	{
		namespace
		{
			struct DumpToFile
			{
				DumpToFile() :
					m_file(
						Files::GetFolderAppDocumentsSettings(
							Globals::g_userFolder,
							L"Log") / (Threads::GetThreadName() + L".txt"s),
						GENERIC_READ | GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						0
					)
				{
				}

				~DumpToFile()
				{
					if (!Exceptions::IsValid())
						Write(L"Exception has been thrown");
				}

				void Write(const wstring& output) noexcept
				{
					m_file.Write(output);
					OutputDebugString(output.c_str());
				}

				ATG::CriticalSectionLockable m_lock;
				Files::SmartHandleFile m_file;
			};

			DumpToFile& LogFile()
			{
				thread_local DumpToFile s_logFile;
				return s_logFile;
			}
		}

		wstring TraceParam(
			const wstring& param,
			const wstring& value
		)
		{
			return param + L": " + value;
		}

		Trace::Trace(
			const wstring& stage,
			InitializerListString params,
			InitializerListString severities
		) :
			m_startTime{ Time::g_fTime },
			c_stage{ L"*" + stage + L"*" },
			m_severity{ false },
			c_exception{ InAnyCloseReason() },
			m_params{ params }
		{
			if (g_severities.empty())
				m_severity = true; // Log all
			else if (severities.empty())
				m_severity = true; // Log me always
			else
			{
				VectorString intersection;
				m_severity = !ranges::set_intersection_result(
					g_severities,
					severities,
					intersection
				).out.empty();
			}

			m_params.emplace_back(
				TraceParam(L"Start Time", 
					m_startTime)
			);
			m_params.emplace_back(
				TraceParam(L"This Thread", 
					GetCurrentThreadId())
			);
			WriteToConsole(L"Begin-");
		}

		Trace::~Trace()
		{
			if (InAnyCloseReason() != c_exception)
				m_params.emplace_back(TraceParam(L"Exception", L"Was Thrown"));
			const double endTime(Time::g_fTime);
			m_params.emplace_back(TraceParam(L"Stop Time", endTime));
			SYSTEMTIME traceTime;
			GetLocalTime(&traceTime);
			m_params.emplace_back(TraceParam(L"Local Clock", traceTime));
			if (endTime - m_startTime >= 1)
				m_params.emplace_back(TraceParam(L"Work Time", endTime - m_startTime));
			WriteToConsole(L"End-");
		}

		void Trace::WriteToConsole(const wstring& log) const
		{
			if (m_severity)
			{
				LogFile().m_lock.lock();
				wstring parameters{ log + c_stage + L'\n' };
				for (const auto OneParam : m_params)
					parameters += OneParam + L'\n';
				parameters += L'\n';
				LogFile().Write(parameters);
				LogFile().m_lock.unlock();
			}
		}

		Trace::VectorString g_severities;
		/// <summary>
		/// 
		/// </summary>
		/// <param name="log"></param>
		void WriteToErrorLog(const std::wstring& log)
		{
			LogFile().Write(L"\n");
			//LogFile().Write(L"Thread Id: ");
			//LogFile().Write(to_wstring(GetCurrentThreadId()));
			//LogFile().Write(Threads::GetThreadName());
			//LogFile().Write(L"; ");
			LogFile().Write(log);
			//LogFile().Write(L"\n");
			//LogFile().Flush();
		}

		void WriteToErrorLogRaw(const wchar_t* pLog)
		{
			LogFile().Write(L"\n");

			SYSTEMTIME fileTime;
			GetLocalTime(&fileTime);
			wstring str(StringFromType(fileTime));
			str += L' ';
			str += Threads::GetThreadName();
			str += L' ';
			str += pLog;
			LogFile().Write(str);
		}
	}
}
#endif