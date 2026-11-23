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
	std::wstring g_isXMLValid;
}

const std::wstring& IsReturnedXMLStatus() noexcept
{
	return g_isXMLValid;
}

LoaderXMLStatus::LoaderXMLStatus(
	IStream* const resultXMLStream
) :
	XMLManager(
		resultXMLStream,
		{
		{ L"status",
		[](const XMLElement& tag, XMLManager*)
		{
			g_isXMLValid = tag.GetAttr(L"code").c_str();
		}} }
	)
{
	Processor();
}