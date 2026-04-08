// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string_view>

namespace Monad::Threads
{
	constexpr std::wstring_view
		THREAD_NAME_SINGLETON_POOL = L"singleton_pool",
		THREAD_NAME_AUDIO_STREAM_POOL = L"audio_pool",
		THREAD_NAME_XBOX_CONTROLLER = L"xbox_controller";
}