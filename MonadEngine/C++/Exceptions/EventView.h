// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>

// STD
#include <stdint.h>
#include <string>

namespace Monad::Exceptions
{
	/// <summary>
	/// This event log functionality does not throw exceptions
	/// </summary>
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
		EventLog(const EventLog&) = delete;
		EventLog& operator=(const EventLog&) = delete;
		EventLog(EventLog&&) = delete;
		EventLog& operator=(EventLog&&) = delete;

		/// <summary>
		/// Reports a text to the event log.
		/// </summary>
		/// <param name="type">A kind of report text, for example EVENTLOG_WARNING_TYPE</param>
		/// <param name="message">A bulk of rows separated by CRLF values</param>
		void ReportEventGeneric(
			const uint16_t type,
			const std::wstring& message
		) const noexcept;

		/// <summary>
		/// Reports a success to the event log.
		/// </summary>
		/// <param name="message">A bulk of rows separated by CRLF values</param>
		void ReportSuccessEvent(
			const std::wstring& message
		) const noexcept;

		/// <summary>
		/// Reports an error to the event log.
		/// </summary>
		/// <param name="message">A bulk of rows separated by CRLF values</param>
		void ReportErrorEvent(
			const std::wstring& message
		) const noexcept;

	private:
		const HANDLE c_hEventLog;
	};
}