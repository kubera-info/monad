// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

// Monad
#include "Kernel/Del.h"

namespace Monad::Exceptions
{
	struct EventLog
	{
		/// <summary>
		/// A class boxing Event Viewer app's report
		/// </summary>
		/// <param name="fullAppName">An application's title for the header of the Event Viewer</param>
		EventLog(
			const std::wstring& fullAppName
		) noexcept;
		~EventLog();
		OPER_DEL(EventLog);
		/// <summary>
		/// Reports an error to event log.
		/// </summary>
		/// <param name="message">A bulk of rows separated by CRLF values</param>
		void ReportErrorEvent(
			const std::wstring& message
		) const noexcept;

	private:
		const HANDLE c_hEventLog;
	};
}