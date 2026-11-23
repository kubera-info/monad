// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>
#include <string_view>

namespace Monad::Threads
{
	constexpr std::wstring_view
		THREAD_NAME_SINGLETON_POOL = L"Singleton Pool",
		THREAD_NAME_AUDIO_STREAM_POOL = L"Audio Stream Pool",
		THREAD_NAME_XBOX_CONTROLLER = L"XBOX Controller";
	
	std::wstring GetThreadName();
}