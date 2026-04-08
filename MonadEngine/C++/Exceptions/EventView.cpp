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

// Monad
#include "Kernel/Patterns.h"
#include "Exceptions.h"

using namespace std;

namespace Monad::Exceptions
{
	namespace
	{
		void ReportEventGeneric(
			const HANDLE hEventLog,
			const WORD type,
			const std::wstring& message
		) noexcept
		{
			if (hEventLog)
				try
				{
					Kernel::VectorWString parts;
					wstringstream wss(message);
					for (wstring currentRow; getline(wss, currentRow);)
						if (!currentRow.empty())
							parts.emplace_back(move(currentRow));
					assert(!parts.empty());
					auto stringZRows(
						ranges::views::transform(
							parts,
							[](const auto& str) noexcept { return str.c_str(); }) |
						ranges::to<vector<const wchar_t*>>());
					::ReportEvent(
						hEventLog,
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
				MONAD_CATCH
				{
				}
		}
	}
	EventLog::EventLog(
		const wstring& fullAppName
	) noexcept :
		c_hEventLog{
		RegisterEventSource(
			nullptr,
			fullAppName.data())
	}
	{}

	EventLog::~EventLog()
	{
		if (c_hEventLog)
			DeregisterEventSource(c_hEventLog);
	}

	void EventLog::ReportErrorEvent(
		const wstring& message
	) const noexcept
	{
		ReportEventGeneric(
			c_hEventLog,
			EVENTLOG_ERROR_TYPE,
			message
		);
	}
}