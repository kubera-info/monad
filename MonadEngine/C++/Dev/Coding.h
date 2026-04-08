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
	const std::wstring ToPascalOrCamelCase(
		const std::wstring& name,
		const DEV_ENCODE isPascal = DEV_ENCODE::PASCAL_CASE
	);
}
