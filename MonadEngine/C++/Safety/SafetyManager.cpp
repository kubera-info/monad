// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "SafetyManager.h"
// Monad
#include "Files/Struct.h"
#include "Globals/__MonadGlobals.h"

namespace fs = std::filesystem;

namespace Monad::Safety
{
	ArrayKey256Bits SafetyState::HashSource(
		InitializerListArgHash buffers
	)
	{
		return m_sha256Manager.HashSource(buffers);
	}

	void SafetyState::GetEncrypted(
		const fs::path& key,
		Files::VectorBytesArchive& source
	)
	{
		m_aes256Manager.GetEncrypted(
			m_sha256Manager,
			key,
			source
		);
	}

	void SafetyState::GetClear(
		const fs::path& key,
		Files::VectorBytesArchive& source
	)
	{
		source.ValidateBufferIn();
		m_aes256Manager.GetClear(
			m_sha256Manager,
			key,
			source
		);
		source.ValidateBufferOut(
			*this,
			key
		);
	}
}