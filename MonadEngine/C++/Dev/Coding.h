// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>

namespace Monad::Dev
{
	enum DEV_ENCODE : bool
	{
		PASCAL_CASE = true,
		CAMEL_CASE = false
	};
	/// <summary>
	/// Transforms naming conventions: SCREAMING_SNAKE_CASE / snake_case → camelCase / PascalCase.
	/// By convention, types are named in PascalCase and variables in camelCase.
	/// </summary>
	/// <param name="name">Any snake case name</param>
	/// <param name="called">'pascal case' or 'camel case'</param>
	/// <returns>Transformed case</returns>	
	template<typename C>
	const std::basic_string<C> ToPascalOrCamelCase(
		const std::basic_string<C>& name,
		const DEV_ENCODE isPascal = DEV_ENCODE::PASCAL_CASE
	)
	{
		std::basic_string<C> newCaseName;
		bool capitalizeNext = isPascal;

		for (const C ch : name)
			if (uint32_t('_') == uint32_t(ch))
				capitalizeNext = true;
			else if (capitalizeNext)
			{
				capitalizeNext = false;
				newCaseName += static_cast<C>(toupper(ch));
			}
			else
				newCaseName += static_cast<C>(tolower(ch));

		return newCaseName;
	}
}
