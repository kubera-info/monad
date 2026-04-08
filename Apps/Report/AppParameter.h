#pragma once

// Monad
#include "Files/XML.h"
#include "Inet/BoxedCurl.h"

struct XMLManagerErrorDesc : Monad::Files::XMLManager
{
	XMLManagerErrorDesc(IStream* const fileStream);
	std::wstring GetParameter(const std::wstring& parameterName) const;
	Monad::Internet::CurlINet::QueryParameter QueryParameter(const std::wstring& parameterName) const;
	//std::wstring Obfuscate(const std::wstring& parameterName) const;
	std::wstring GetErrorMessageFromHRESULT() const noexcept;
};
extern std::unique_ptr<XMLManagerErrorDesc> g_parameters;
