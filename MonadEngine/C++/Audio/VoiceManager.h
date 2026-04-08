// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <queue>
// Monad
#include "Voice.h"
#include "WaveWithCallback.h"
#include "Time/TimeLerp.h"

namespace Monad::Audio
{
	/// <summary>
	/// Voice Manager encapsulates an audio queue that manages multiple voice types.
	/// At one moment only one Wave could be played in the manager
	/// </summary>
	struct VoiceManager : IXAudio2VoiceCallback, Time::TimeLerp
	{
		VoiceManager(
			IXAudio2* const xAudio2,
			const std::string& queue
		);

		void OnFrameMove() noexcept final;
		void OnFlush() noexcept override;
		void Mute() noexcept;

		/// <summary>
		/// Determines if any voices in the Voice Manager are actively playing.
		/// </summary>
		/// <returns>Returns true if a voice is currently playing, or if there are buffers queued for playback.</returns>
		bool IsPlaying() const noexcept;
		/// <summary>
		/// Waits while the Voice Manager is actively playing.
		/// </summary>
		void WaitForEndOfStreams() const noexcept;
		/// <summary>
		/// Stops playback of the voice and clears its buffer queue.
		/// </summary>
		/// <returns>HRESULT status</returns>
		HRESULT ResetManager() noexcept;
		HRESULT AudioEnqueueSound(
			const WaveToPlayWithCallback& wave
		);
		HRESULT AudioDequeueWave();
		void SetVoiceVolume(
			const float volume
		);
		void Flush() noexcept;

		// Internals
		HRESULT AudioPlayWave(
			WaveToPlayWithCallback& pendingWave
		);
		void OnAfterWave();
		void SetVoiceVolumeGeneric(
			const float volume
		);
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnVoiceProcessingPassStart(
			uint32_t
		) final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnStreamEnd() final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnBufferStart(
			void*
		) final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnBufferEnd(
			void*
		) final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnLoopEnd(
			void*
		) final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnVoiceError(
			void*,
			HRESULT
		) final;

		std::atomic_bool m_isQueueErased = false;

	protected:
		inline HRESULT PlayNewVoice(
			WaveBuffer* const wave
		);
		template<typename Self>
		auto GetPlayingVoice(this Self&& self) noexcept
		{
			const auto ifFound = std::ranges::find_if(
				self.m_distinctVoices,
				[](const auto& voice) noexcept { return voice.second.IsPlaying(); });

			return ifFound != self.m_distinctVoices.cend() ? &ifFound->second : nullptr;
		}

		IXAudio2* m_xAudio2 = nullptr;
		std::queue<WaveToPlayWithCallback> m_voiceCommandQueue;
		WaveToPlayWithCallback m_currentWave;

		// All voices managed within a single Voice Manager.
		std::unordered_map <const WAVEFORMATEX*, Voice> m_distinctVoices;
		std::string m_queue;
	};
}
