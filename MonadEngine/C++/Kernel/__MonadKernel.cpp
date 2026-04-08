// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadKernel.h"

namespace Monad::Kernel
{
	/*std::wstring SystemItem(const std::wstring& name)
	{
		assert(!IsSystemItem(name));
		return L":" + name;
	}
	std::wstring NormalItem(const std::wstring& name)
	{
		assert(IsSystemItem(name));
		return name.substr(1);
	}
	bool IsSystemItem(const std::wstring& name) noexcept
	{
		return name.starts_with(L':');
	}*/
	const std::string GHOST(":g");
}