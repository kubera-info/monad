// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Trace.h"

#if defined(MONAD_TEST)
// Platform
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"

// STD
#	include <fstream>

// Monad
#	include "Files/Folder.h"
#	include "Kernel/Patterns.h"
#	include "Time/__MonadTime.h"

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
				DumpToFile()
				{
					m_fileName = Files::GetFolderAppDocumentsSettings(L"Test", L"04.01.2024") / (L"MonadLog "s + Threads::GetThreadName() + L".txt"s);
					DeleteFile(m_fileName.data());
					m_file.open(m_fileName);
				}

				~DumpToFile()
				{
					if (!Exceptions::IsValid())
						write_sz(L"Exception has been thrown");
				}

				void write(const wstring& output) noexcept
				{
					write_sz(output.c_str());
				}

				void write_sz(const wchar_t* output) noexcept
				{
					m_file << output;
					OutputDebugString(output);
				}

				void flush()
				{
					m_file.flush();
				}

				ATG::CriticalSectionLockable m_lock;
				wofstream m_file;
				wstring m_fileName;
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
				LogFile().write(parameters);
				LogFile().flush();
				LogFile().m_lock.unlock();
			}
		}

		Trace::VectorString g_severities;

		void WriteToErrorLog(const wchar_t* log)
		{
			LogFile().write_sz(L"\n");
			LogFile().write_sz(L"Thread Id: ");
			LogFile().write(to_wstring(GetCurrentThreadId()));
			LogFile().write(Threads::GetThreadName());
			LogFile().write_sz(L"; ");
			LogFile().write_sz(log);
			LogFile().write_sz(L"\n");
			LogFile().flush();
		}

		void WriteToErrorLogRaw(const wchar_t* pLog)
		{
			LogFile().write_sz(L"\n");

			SYSTEMTIME fileTime;
			GetLocalTime(&fileTime);
			wstring str(StringFromType(fileTime));
			str += L' ';
			str += Threads::GetThreadName();
			str += L' ';
			str += pLog;
			LogFile().write(str);
		}
	}
}
#endif