// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
#include <variant>
// Monad
#include "Kernel/BytesVector.h"
#include "Kernel/Del.h"
#include "__MonadSafety.h"

//
// The hashing API for assotiative conteiners and encryption's salts
//

namespace Monad::Safety
{
	using PtrString = const std::string*;
	using PtrWString = const std::wstring*;
	using PtrSpanBytes = const Kernel::SpanBytes*;
	using PtrPath = const std::filesystem::path*;
	using InitializerListArgHash = std::initializer_list<std::variant<PtrString, PtrWString, PtrSpanBytes, PtrPath>>;

	namespace SafetyCore
	{
		/// <summary>
		/// A SHA256 hash algorithm + data container
		/// </summary>
		struct SHA256Manager final : SafetyManager
		{
			SHA256Manager();
			~SHA256Manager() = default;
			OPER_DEL_NO_DEF_CTOR(SHA256Manager);
			/// <summary>
			/// Computes a SHA256 hash
			/// </summary>
			/// <param name="buffers">Containter of a binary buffer</param>
			/// <returns>SHA256 hash</returns>
			ArrayKey256Bits HashSource(
				InitializerListArgHash buffers
			);
		};
	}
}