// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "EventView.h"

// Platform
#pragma comment(lib, "Advapi32.lib")

// STD
#include <ranges>

using namespace std;

namespace Monad::Exceptions
{
	EventLog::EventLog(
		const wstring& fullAppName
	) noexcept :
		c_hEventLog{
		RegisterEventSourceW(
			nullptr,
			fullAppName.data())
	}
	{}

	EventLog::~EventLog()
	{
		if (c_hEventLog)
			DeregisterEventSource(c_hEventLog);
	}

	void EventLog::ReportEventGeneric(
		const uint16_t type,
		const wstring& message
	) const noexcept
	{
		if (c_hEventLog)
		{
			try
			{
				vector<wstring> parts;
				wstringstream wss(message);
				for (wstring currentRow; getline(wss, currentRow);)
					parts.emplace_back(currentRow);
				auto stringZRows(
					ranges::views::transform(
						parts,
						[](const auto& str) noexcept { return str.c_str(); }) |
					ranges::to<vector<const wchar_t*>>());
				assert(stringZRows.size() <= MAXUINT16);
				ReportEvent(
					c_hEventLog,
					type,
					0,
					0,
					nullptr,
					static_cast<WORD>(stringZRows.size()),
					0,
					stringZRows.data(),
					nullptr
				);
			}
			catch (...)
			{
				abort();
			}
		}
	}

	void EventLog::ReportSuccessEvent(
		const wstring& message
	) const noexcept
	{
		ReportEventGeneric(
			EVENTLOG_SUCCESS,
			message
		);
	}

	void EventLog::ReportErrorEvent(
		const wstring& message
	) const noexcept
	{
		ReportEventGeneric(
			EVENTLOG_ERROR_TYPE,
			message
		);
	}
}
