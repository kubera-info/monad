// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Op.h"
#include "Struct.h"

namespace Monad::Files
{
	/// <summary>
	/// Exports in-memory assets to compressed and encrypted files.
	/// </summary>
	/// <typeparameter name="V">A container of blocks, such as vector, wstring, etc./typeparameter>
	/// <param name="cipher">Cipher context</param>
	/// <param name="buffer">Input clear buffer</param>
	/// <param name="outputPath">Output path</param>
	template<typename V>
	void CompressEncryptAndSaveFromMemory(
		Safety::SafetyState& cipher,
		const std::filesystem::path& outputPath,
		const V& buffer
		)
	{
		const auto key = outputPath.filename();
		VectorBytesArchive compressed(
			buffer,
			cipher,
			key
		);
		cipher.GetEncrypted(
			key,
			compressed
		);
		SmartHandleFile{
			outputPath,
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			g_writeParameters.dwFileAttributesAndFlags
		}
		.Write(compressed);
	}
}
