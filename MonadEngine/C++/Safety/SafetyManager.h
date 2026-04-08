// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Safety/Cipher.h"
#include "Safety/Hash.h"

namespace Monad::Safety
{
	struct SafetyState final
	{
		/// <summary>
		/// Calculates SHA256 from Buffers
		/// </summary>
		/// <param name="pBuffers">A list of input buffers</param>
		/// <returns>Calculated hash</returns>
		ArrayKey256Bits HashSource(
			InitializerListArgHash buffers
		);
		/// <summary>
		/// Encrypts source
		/// </summary>
		/// <param name="key">A filaname as a part of the dest. key</param>
		/// <param name="source">A clean buffer</param>
		/// <param name="salt">A global, unique salt for this app.</param>
		void GetEncrypted(
			const std::filesystem::path& key,
			Files::VectorBytesArchive& source
		);
		/// <summary>
		/// Decrypts source
		/// </summary>
		/// <param name="key">A filaname as a part of the dest. key</param>
		/// <param name="source">An encrypted buffer (must be: File::VectorBytesArchive)</param>
		/// <param name="salt">A global salt for this app.</param>
		void GetClear(
			const std::filesystem::path& key,
			Files::VectorBytesArchive& source
		);

	private:
		/// <summary>
		/// Calculates SHA256
		/// </summary>
		SafetyCore::SHA256Manager m_sha256Manager;
		/// <summary>
		/// Encrypts and decrypts using AES256
		/// </summary>
		SafetyCore::AES256Manager m_aes256Manager;
	};
}