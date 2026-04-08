// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Voice.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "Persistent.h"

namespace Monad::Audio
{
#pragma region Voice
	Voice::Voice(
		IXAudio2* xAudio2,
		IXAudio2VoiceCallback* callback,
		float volume,
		const WAVEFORMATEX* waveFormatEx
	) :
		m_pSourceVoice{
			nullptr,
			DestroyVoice
	}
	{
		assert(nullptr != xAudio2);
		assert(nullptr != waveFormatEx);
		IXAudio2SourceVoice* tmpSourceVoice;
		THROW_EXC_AUDIO(xAudio2->CreateSourceVoice(
			&tmpSourceVoice,
			waveFormatEx,
			0,
			XAUDIO2_DEFAULT_FREQ_RATIO,
			callback), L"Create Source Voice");
		m_pSourceVoice.reset(tmpSourceVoice);
		if (VOLUME_MAX != volume)
			SetVolume(volume);
	}

	HRESULT Voice::PlayWave(
		const WaveBuffer* const wave
	) const noexcept
	{
		// Validates the buffer before submission.
		if (!wave || wave->c_bufferCPU.size() == 0 || wave->c_bufferCPU.data() == nullptr)
			return E_INVALIDARG;

		// Enqueues WAV sample data for playback via an XAUDIO2_BUFFER structure.
		const XAUDIO2_BUFFER buffer{ 0, static_cast<uint32_t>(wave->c_bufferCPU.size()), wave->c_bufferCPU.data(), 0, 0, 0, 0, 0, nullptr };

		V_RETURN(m_pSourceVoice->SubmitSourceBuffer(&buffer));
		return m_pSourceVoice->Start();
	}

	void Voice::SetVolume(
		const float volume
	) noexcept
	{
		m_pSourceVoice->SetVolume(volume);
	}

	HRESULT Voice::FlushSourceBuffers(
		const std::string& queue
	) noexcept
	{
		if (!g_persistentAudio->IsReady())
			return S_OK;
		assert(nullptr != m_pSourceVoice);
		SetVolume(VOLUME_MUTED);
		V_RETURN(m_pSourceVoice->Stop(0));
		V_RETURN(m_pSourceVoice->FlushSourceBuffers());
		WaitForEndOfStreams();
		SetVolume(g_persistentAudio->GetVolume(queue));
		return S_OK;
	}

	bool Voice::IsPlaying() const noexcept
	{
		XAUDIO2_VOICE_STATE state{};
		assert(m_pSourceVoice);
		m_pSourceVoice->GetState(&state);
		return 0 < state.BuffersQueued;
	}

	void Voice::WaitForEndOfStreams() const noexcept
	{
		while (g_persistentAudio->IsReady() && IsPlaying())
			SwitchToThread();
	}
#pragma endregion Voice
}
