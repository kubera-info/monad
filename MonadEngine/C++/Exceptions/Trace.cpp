// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// ############################################################################
// ### Copyright © 2016-2024 by W³odzimierz O. Kubera. All rights reserved. ###
// ############################################################################

#include "stdafx.h"
#include "MonadTrace.h"
//
#if defined(MONAD_TEST)
#	include <fstream>
#	include <mutex>
#	include "Files/MonadFileFolder.h"
#	include "Kernel/MonadPatterns.h"
#	include "Time/__MonadTime.h"
#	include "Threads/MonadThreadName.h"

using namespace std;

namespace Monad
{
	using namespace Kernel;
	using namespace Threads;

	namespace Exceptions
	{
		struct DumpToFile
		{
			DumpToFile()
			{
				m_fileName = Files::GetFolderAppDocumentsSettings(L"Test", L"04.01.2024") / (L"MonadLog "s + Threads::g_threadName.data() + L".txt"s);
				DeleteFile(m_fileName.data());
				m_file.open(m_fileName);
			}

			~DumpToFile()
			{
				if (!Exceptions::IsValid())
					write_sz(L"Exception has been thrown");
			}

			void write(const wstring& pOutput)
			{
				write_sz(pOutput.c_str());
			}

			void write_sz(const wchar_t* pOutput)
			{
				m_file << pOutput;
				System::MndOutputDebugString(pOutput);
			}

			void flush()
			{
				m_file.flush();
			}

			mutex m_lock;
			wofstream m_file;
			wstring m_fileName;
		};

		DumpToFile& LogFile()
		{
			thread_local DumpToFile s_logFile;
			return s_logFile;
		}

		wstring TraceParam(const wstring& pParam, const wstring& pValue)
		{
			return pParam + L": " + pValue;
		}

		Trace::Trace(const wstring& pStage, InitListString pParams, InitListString pSeverities) :
			m_startTime{ Time::g_fTime }, c_stage{ L"*" + pStage + L"*" }, /*m_params(pParams),*/ m_severity{ false }, c_exception{ InAnyCloseReason() },
			m_params{ pParams.begin(), pParams.end() }
		{
			if (gSeverities.empty())
				m_severity = true; // Log all
			else if (0 == pSeverities.size())
				m_severity = true; // Log me always
			else
			{
				Kernel::VectorString intersection;
				m_severity = intersection.cend() != set_intersection(gSeverities.cbegin(), gSeverities.cend(), pSeverities.begin(), pSeverities.end(), intersection.begin());
			}

			m_params.emplace_back(TraceParam(L"Start Time", m_startTime));
			m_params.emplace_back(TraceParam(L"This Thread", GetCurrentThreadId()));
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

		void Trace::WriteToConsole(const wstring& pLog) const
		{
			if (m_severity)
			{
				LogFile().m_lock.lock();
				wstring tmp{ pLog + c_stage + L'\n' };
				for (const auto OneParam : m_params)
					tmp += OneParam + L'\n';
				tmp += L'\n';
				LogFile().write(tmp);
				LogFile().flush();
				LogFile().m_lock.unlock();
			}
		}

		Kernel::VectorString gSeverities;

		void WriteToErrorLog(const wchar_t* pLog)
		{
			LogFile().write_sz(L"\n");
			LogFile().write_sz(L"Thread Id: ");
			LogFile().write(to_wstring(GetCurrentThreadId()));
			LogFile().write(Threads::g_threadName.data());
			LogFile().write_sz(L"; ");
			LogFile().write_sz(pLog);
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
			str += Threads::g_threadName;
			str += L' ';
			str += pLog;
			LogFile().write(str);
		}
	}
}
#endif