// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <xaudio2.h>
// STD
#include <memory>
// Monad
#include "__MonadAudio.h"
#include "WaveBuffer.h"

namespace Monad::Audio
{
	struct PersistentAudio;

	/// <summary>
	/// Represents one source voice through the IXAudio2SourceVoice interface.
	/// </summary>
	struct Voice final
	{
		Voice(
			IXAudio2* xAudio2,
			IXAudio2VoiceCallback* callback,
			float volume,
			const WAVEFORMATEX* waveFornatEx
		);
		OPER_DEL(Voice);
		/// <summary>
		/// Plays a WAV buffer that is already loaded into memory.
		/// </summary>
		/// <param name="wave">Data structure configured and ready for audio playback.</param>
		/// <returns>Status of playback</returns>
		HRESULT PlayWave(
			const WaveBuffer* const wave
		) const noexcept;
		/// <summary>
		/// Sets the volume of an IXAudio2SourceVoice voice.
		/// </summary>
		/// <param name="volume">Volume level of the voice, expressed as a float between 0.0f (silent) and 1.0f (full volume).</param>
		void SetVolume(
			const float volume
		) noexcept;
		/// <summary>
		/// Clears the specified voice interface and removes all buffers currently queued for playback.
		/// </summary>
		/// <returns>Should return S_OK</returns>
		HRESULT FlushSourceBuffers() noexcept;
		/// <summary>
		/// Checks whether the current voice is playing or has buffers queued for playback.
		/// </summary>
		/// <returns>true if sth is being played</returns>
		bool IsPlaying() const noexcept;
		/// <summary>
		/// Blocks execution until the audio playback completes.
		/// </summary>
		void WaitForEndOfStreams() const noexcept;

	private:
		std::unique_ptr<IXAudio2SourceVoice, FnDestroyVoice> m_sourceVoice;
	};
}
