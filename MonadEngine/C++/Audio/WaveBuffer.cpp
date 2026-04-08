// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "WaveBuffer.h"

namespace Monad
{
	using namespace Kernel;

	namespace Audio
	{
		WaveBuffer::WaveBuffer(
			const WAVEFORMATEX& waveFornatEx,
			VectorBytes&& buffer,
			SetWaveFormatEx& formats
		) :
			c_waveFornatEx{ GetWaveFormatEx(
				waveFornatEx,
				formats) },
			c_bufferCPU{ std::move(buffer) }
		{}

		const WAVEFORMATEX* WaveBuffer::GetWaveFormatEx(
			const WAVEFORMATEX& possiblyNewFormat,
			SetWaveFormatEx& formats
		)
		{
			return &*formats.emplace(possiblyNewFormat).first;
		}
	}
}