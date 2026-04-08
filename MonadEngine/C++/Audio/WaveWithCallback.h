// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "WaveBuffer.h"

namespace Monad::Audio
{
	struct PersistentAudio;

	/// <summary>
	/// This interface can be implemented to play WAV audio, for example, to support automatic streaming.
	/// </summary>
	interface IWaveCallback abstract
	{
		virtual void OnAfterWave() noexcept = 0;
	};

	/// <summary>
	/// Encapsulates Wave PCM data and a callback within one object, able to handle both existing and incoming WAV buffers.
	/// </summary>
	struct WaveToPlayWithCallback final
	{
		WaveToPlayWithCallback() noexcept;
		WaveToPlayWithCallback(
			const std::string& filename,
			IWaveCallback* const callback,
			const PtrWaveBuffer& waveBuffer
		);
		~WaveToPlayWithCallback() = default;
		OPER_DEF_NO_DEF_CTOR(WaveToPlayWithCallback);
		bool IsCallback() const noexcept;
		const std::string& GetFilename() const noexcept;
		bool empty() const noexcept;
		// void BeforeWave();
		void OnAfterWave() noexcept;
		const PtrWaveBuffer& data() const noexcept;
		bool IfPlay() const noexcept;
		void InvalidatePlayOption() noexcept;
		// Checks whether both buffers are compatible with the same voice.
		bool IsCompatible(
			const WaveToPlayWithCallback& toCompare
		) const noexcept;

	private:
		std::string m_filename;
		IWaveCallback* m_callback;
		PtrWaveBuffer m_waveBuffer;
		bool m_ifEnqueAndInvalidate;
	};
}
