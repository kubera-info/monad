// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/ComManager.h"
#include "Kernel/Templates.h"

namespace Monad::Audio
{
	/// <summary>
	/// Contains WAVEFORMATEX structures and exposes them via pointers (WAVEFORMATEX*) to improve performance. Each value is referenced by its pointer.
	/// </summary>
	using SetWaveFormatEx = Kernel::UnorderedSet<WAVEFORMATEX>;

	/// <summary>
	/// A single WAV audio buffer ready for playback.
	/// </summary>
	struct WaveBuffer final : Kernel::RefCount
	{
		WaveBuffer(
			const WAVEFORMATEX& waveFornatEx,
			Kernel::VectorBytes&& buffer,
			SetWaveFormatEx& formats
		);

		const WAVEFORMATEX* c_waveFornatEx;
		const Kernel::VectorBytes c_bufferCPU;

	private:
		inline const WAVEFORMATEX* GetWaveFormatEx(
			const WAVEFORMATEX& possiblyNewFormat,
			SetWaveFormatEx& formats
		);
	};

	/// <summary>
	/// PCM WAV buffer adapted for the audio engine, using a COM-style interface for access and management.
	/// </summary>
	using PtrWaveBuffer = Microsoft::WRL::ComPtr<WaveBuffer>;
}
