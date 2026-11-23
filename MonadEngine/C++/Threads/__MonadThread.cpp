// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadThread.h"

// Monad
#include "Exceptions/Exceptions.h"

using namespace std;

namespace Monad::Threads
{
	namespace
	{
		const std::wstring UNNAMED_THREAD = L"Unnamed Thread";
	}
	std::wstring GetThreadName()
	{
		LPWSTR threadNameBuffer = nullptr;
		if (SUCCEEDED(GetThreadDescription(
			GetCurrentThread(),
			&threadNameBuffer))
			)
		{
			std::wstring threadName = threadNameBuffer;
			LocalFree(threadNameBuffer);
			return threadName.empty() ? UNNAMED_THREAD : threadName;
		}
		else
			return UNNAMED_THREAD;
	}
}