// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "BugXML.h"

// Platform
#include "resource.h"

namespace
{
	std::wstring g_IsXMLValid;
}

const std::wstring& IsReturnedXMLStatus() noexcept
{
	return g_IsXMLValid;
}

LoadXMLStatus::LoadXMLStatus(
	IStream* const resultXMLStream
) :
	XMLManager(
		resultXMLStream,
		{
		{ L"Log",
		[](const XMLElement& tag, XMLManager*)
		{
			g_IsXMLValid = tag.GetAttr(L"log_id").c_str();
			/*const auto wnd = theApp.GetMainWnd();
			wnd->GetDlgItem(IDC_LABEL_REP_ID)->ShowWindow(TRUE);
			wnd->GetDlgItem(IDI_CPY)->ShowWindow(TRUE);
			const auto logID = wnd->GetDlgItem(IDC_REP_ID);
			logID->SetWindowText(tag.GetAttr(L"log_id").c_str());
			logID->ShowWindow(TRUE);*/
		}} }
	)
{
	Processor();
}