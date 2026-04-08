// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppParameter.h"

// Platform
#include <comdef.h>

// Monad
#include "Globals/__MonadGlobals.h"
#include "I18N/__MonadI18N.h"
#include "System/Parameter.h"
#include "Tools/Convert.h"

using namespace std;
using namespace Monad::Globals;
using namespace Monad::I18N;
using namespace Monad::Internet;
using namespace Monad::System;
using namespace Monad::Tools;

#pragma region XMLManagerErrorDesc
wstring XMLManagerErrorDesc::GetParameter(
	const wstring& parameterName
) const
{
	return m_rootOfDoc.GetAttr(parameterName);
}

CurlINet::QueryParameter XMLManagerErrorDesc::QueryParameter(
	const wstring& parameterName
) const
{
	return { UnicodeToUTF8(parameterName), GetParameter(parameterName) };
}

/*std::wstring XMLManagerErrorDesc::Obfuscate(
	const std::wstring& parameterName
) const
{
	return Monad::System::Obfuscate(GetParameter(parameterName));
}*/

XMLManagerErrorDesc::XMLManagerErrorDesc(
	IStream* const fileStream
) :
	XMLManager(fileStream, {})
{
	SetLocaleName(
		UnicodeToUTF8(GetParameter(PAR_NAME_I18N))
	);
	g_sourceModuleHandle = LoadLibraryEx(
		GetParameter(PAR_NAME_PATH).c_str(),
		nullptr,
		LOAD_LIBRARY_AS_DATAFILE
	);
	g_sourceIcon = static_cast<WORD> (std::wcstol(
		GetParameter(PAR_NAME_ICO).c_str(),
		nullptr,
		10)
		);
}

wstring XMLManagerErrorDesc::GetErrorMessageFromHRESULT() const noexcept
{
	HRESULT hr = E_FAIL;
	try
	{
		hr = stoul(GetParameter(PAR_NAME_HRESULT), nullptr, 16);
	}
	catch (invalid_argument&)
	{
	}
	return _com_error{ hr }.ErrorMessage();
}

unique_ptr<XMLManagerErrorDesc> g_parameters;
#pragma endregion