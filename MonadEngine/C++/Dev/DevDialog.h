// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#include "GUI/Dialog.h"

namespace Monad::Dev
{
	struct DialogFullScreenDev final : GUI::DialogFullScreenGeneric
	{
		DialogFullScreenDev();
		void AppendOutputString(std::wstring logString);
		void AppendOutputI18NString(const I18N::I18NString& logString);
		void AppendNewLine();
		void AppendCurrentTime();
		void AppendTime(SYSTEMTIME timestamp);
		void AppendAppVersionAndArch();
		void ClearString() noexcept;

	private:                                                         
		std::wstring m_currentLog;
	};
}