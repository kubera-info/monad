// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "DevDialog.h"

// Monad
#include "Pages/__MonadPage.h"
#include "System/System.h"
#include "Tools/Convert.h"

namespace Monad::Dev
{
	DialogFullScreenDev::DialogFullScreenDev() :
		DialogFullScreenGeneric("console"_pageNo, { L"Console"_en_US })
	{}

	void DialogFullScreenDev::AppendOutputString(std::wstring logString)
	{
		m_currentLog += logString;
	}

	void DialogFullScreenDev::AppendOutputI18NString(const I18N::I18NString& logString)
	{
		AppendOutputString(logString.GetVal());
	}

	void DialogFullScreenDev::AppendNewLine()
	{
		m_currentLog += L'\n';
	}
	void DialogFullScreenDev::AppendCurrentTime()
	{
		SYSTEMTIME timestamp;
		GetLocalTime(&timestamp);
		AppendTime(timestamp);
	}

	void DialogFullScreenDev::AppendTime(SYSTEMTIME timestamp)
	{
		Tools::OutputStringStream SYSTEMTIMEStruct;

		GetLocalTime(&timestamp);
		SYSTEMTIMEStruct << timestamp;
		AppendOutputString(SYSTEMTIMEStruct.str());
	}

	void DialogFullScreenDev::AppendAppVersionAndArch()
	{
		AppendOutputString(System::GetAppVersionAndArch());
	}

	void DialogFullScreenDev::ClearString() noexcept
	{
		m_currentLog.clear();
		m_currentLog.shrink_to_fit();
	}
}