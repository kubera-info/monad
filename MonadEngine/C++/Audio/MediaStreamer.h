// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <mfidl.h>
#include <mfreadwrite.h>
// Monad
#include "Kernel/BytesVector.h"
#include "Kernel/Del.h"

namespace Monad::Audio
{
	/// <summary>
	/// A WAV PCM stream decoded from an MP3 file.
	/// </summary>
	struct MediaStreamer final
	{
		/// <summary>
		/// A stream reader that parses audio data and outputs a WAV PCM buffer for XAudio.
		/// </summary>
		/// <param name="audioFileHolder">Source file provided as a stream (MP3 format preferred).</param>
		MediaStreamer(
			IStream* audioFileHolder
		);
		Kernel::VectorBytes ReadAll();
		const WAVEFORMATEX& GetOutputWaveFormatEx() const noexcept;

	private:
		Microsoft::WRL::ComPtr<IMFSourceReader>	m_reader;
		WAVEFORMATEX m_waveFornatEx;
		uint32_t m_maxStreamLengthInBytes;
	};

	/// <summary>
	/// The Media Foundation platform live manager.
	/// </summary>
	struct MFManager final
	{
		MFManager();
		~MFManager();
		OPER_DEL_NO_DEF_CTOR(MFManager);
	};
}
