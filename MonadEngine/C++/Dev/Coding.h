// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>
#include "Kernel/Templates.h"
#include "Tools/SingleCall.h"
#include "Exceptions/Exceptions.h"

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
		Tools::SingleCall s_profileOnce;
		for (const C ch : name)
		{
			if(!s_profileOnce.test_and_set()) [[unlikely]]
				assert(
					'_' == ch
					|| Kernel::between<size_t>('a', 'z', ch)
					|| Kernel::between<size_t>('A', 'Z', ch)
				); 
			else [[likely]]
				assert(
					'_' == ch
					|| Kernel::between<size_t>('0', '9', ch)
					|| Kernel::between<size_t>('a', 'z', ch)
					|| Kernel::between<size_t>('A', 'Z', ch)
				);
			if ('_' == ch)
				capitalizeNext = true;
			else if (capitalizeNext)
			{
				capitalizeNext = false;
				newCaseName += static_cast<C>(toupper(ch));
			}
			else
				newCaseName += static_cast<C>(tolower(ch));
		}
		return newCaseName;
	}

	inline void EXC() { THROW_EXC(Monad::Exceptions::FileDamaged, E_FAIL, L"test"); }
}
