// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Coding.h"

using namespace std;

namespace Monad::Dev
{
	const wstring ToPascalOrCamelCase(
		const wstring& name,
		const DEV_ENCODE isPascal
	)
	{
		wstring newCaseName;
		bool capitalizeNext = isPascal;

		for (const auto ch : name)
			if (L'_' == ch)
				capitalizeNext = true;
			else if (capitalizeNext)
			{
				newCaseName += static_cast<wchar_t>(toupper(ch));
				capitalizeNext = false;
			}
			else
				newCaseName += static_cast<wchar_t>(tolower(ch));

		return newCaseName;
	}
}