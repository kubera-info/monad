// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
// Monad
#include "Kernel/BytesVector.h"
#include "__MonadSafety.h"

/// <summary>
/// Flags describing the state of a buffer.
/// Flags can be combined (e.g. FLAG_COMPRESSED)
/// </summary>
enum SAFETY_FLAGS : uint8_t
{
	FLAG_NONE = 0,
	FLAG_COMPRESSED = 1 << 0
};

namespace Monad::Safety::SafetyCore
{
	struct SHA256Manager;

	/// <summary>
	/// AES-256 encryption/decryption manager.
	/// Supports encryption, decryption and optional compression.
	/// Internally uses AES256.
	/// </summary>
	struct AES256Manager final : SafetyManager
	{
		AES256Manager();

		/// <summary>
		/// Decrypts buffer in-place.
		/// </summary>
		/// <param name="sha">SHA256 hash used as part of the key</param>
		/// <param name="key">File path used as part of the key</param>
		/// <param name="source">Encrypted buffer (modified in-place)</param>
		void GetClear(
			SHA256Manager& sha,
			const std::filesystem::path& key,
			Files::VectorBytesArchive& source
		);

		/// <summary>
		/// Encrypts buffer in-place.
		/// </summary>
		/// <param name="sha">SHA256 hash used as part of the key</param>
		/// <param name="key">File path used as part of the key</param>
		/// <param name="source">Plain buffer (modified in-place)</param>
		void GetEncrypted(
			SHA256Manager& sha,
			const std::filesystem::path& key,
			Files::VectorBytesArchive& source
		);
	};
}
