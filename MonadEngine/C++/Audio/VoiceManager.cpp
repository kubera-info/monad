// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "VoiceManager.h"
// Monad
#include "Persistent.h"

using namespace std;

namespace Monad::Audio
{
#pragma region VoiceManager
	VoiceManager::VoiceManager(
		IXAudio2* const xAudio2,
		const std::string& queue
	) :
		TimeLerp{
		1.0,
		VOLUME_MAX,
		VOLUME_MUTED },
		m_xAudio2(xAudio2),
		m_queue(queue)
	{}

	HRESULT VoiceManager::ResetManager() noexcept
	{
		HRESULT hr = S_OK;
		const bool isQueueErased = m_isQueueErased;
		m_isQueueErased = true;

		while (!m_voiceCommandQueue.empty())
			m_voiceCommandQueue.pop();
		if (auto playingVoice(GetPlayingVoice()); playingVoice)
			hr = playingVoice->FlushSourceBuffers(m_queue);

		m_isQueueErased = isQueueErased;
		return hr;
	}

	HRESULT VoiceManager::AudioEnqueueSound(
		const WaveToPlayWithCallback& wave
	)
	{
		HRESULT hr = S_OK;
		m_voiceCommandQueue.push(wave);

		if (!IsPlaying())
			hr = AudioDequeueWave();
		if (SUCCEEDED(hr) && !IsPlaying())
			hr = AudioDequeueWave();
		return hr;
	}

	HRESULT VoiceManager::AudioPlayWave(
		WaveToPlayWithCallback& pendingWave
	)
	{
		if (pendingWave.IfPlay())
		{
			assert(!pendingWave.GetFilename().empty());
			pendingWave.InvalidatePlayOption();
			return PlayNewVoice(pendingWave.data().Get());
		}
		return S_OK;
	}

	HRESULT VoiceManager::AudioDequeueWave()
	{
		HRESULT hr = S_OK;

		if (!m_voiceCommandQueue.empty())
		{
			m_currentWave = m_voiceCommandQueue.front();
			m_voiceCommandQueue.pop();
			if (m_currentWave.IfPlay())
			{
				assert(!m_currentWave.empty());
				//auto wave = ValidateBuffer();
				hr = AudioPlayWave(m_currentWave);
			}
			if (SUCCEEDED(hr) && !m_voiceCommandQueue.empty() && !m_currentWave.empty() && m_voiceCommandQueue.front().IsCompatible(m_currentWave))
				return AudioPlayWave(m_voiceCommandQueue.front());
		}
		return hr;
	}

	void VoiceManager::SetVoiceVolumeGeneric(
		const float volume
	)
	{
		if (!g_persistentAudio->IsReady())
			return;
		g_persistentAudio->m_voicesVolumesCache[m_queue] = volume;
		for (auto& OneVoice : m_distinctVoices)
			OneVoice.second.SetVolume(volume);
	}

	void VoiceManager::SetVoiceVolume(
		const float volume
	)
	{
		Flush();
		SetVoiceVolumeGeneric(volume);
	}

	void VoiceManager::Flush() noexcept
	{
		SetVoiceVolumeGeneric(VOLUME_MUTED);
		ResetManager();
		if ("stream" == m_queue)
			g_persistentAudio->InternalClearCurrentStream();
	}

	void VoiceManager::OnAfterWave()
	{
		if (!m_isQueueErased)
		{
			m_currentWave.OnAfterWave();
			AudioDequeueWave();
		}
	}

	void VoiceManager::OnVoiceProcessingPassStart(
		uint32_t
	) noexcept
	{}

	void VoiceManager::OnVoiceProcessingPassEnd() noexcept
	{}

	void VoiceManager::OnStreamEnd() noexcept
	{}

	void VoiceManager::OnBufferStart(
		void*
	) noexcept
	{}

	void VoiceManager::OnBufferEnd(
		void*
	) noexcept
	{
		g_persistentAudio->InternalAfterWave(m_queue);
	}

	void VoiceManager::OnLoopEnd(
		void*
	) noexcept
	{}

	void VoiceManager::OnVoiceError(
		void*,
		HRESULT
	) noexcept
	{
		//Audio::g_persistentAudio->ShutdownOnError();
	}

	bool VoiceManager::IsPlaying() const noexcept
	{
		return nullptr != GetPlayingVoice();
	}

	void VoiceManager::WaitForEndOfStreams() const noexcept
	{
		if (auto playingVoice = GetPlayingVoice(); playingVoice)
			playingVoice->WaitForEndOfStreams();
	}

	inline HRESULT VoiceManager::PlayNewVoice(WaveBuffer* const wave)
	{
		return m_distinctVoices.try_emplace(
			wave->c_waveFornatEx,
			m_xAudio2,
			this,
			g_persistentAudio->m_voicesVolumesCache[m_queue],
			wave->c_waveFornatEx).first->second.PlayWave(wave);
	}

	void VoiceManager::OnFrameMove() noexcept
	{
		if (IsRunning())
			SetVoiceVolumeGeneric(GetCurrentValue());
	}

	void VoiceManager::OnFlush() noexcept
	{
		Flush();
	}

	void VoiceManager::Mute() noexcept
	{
		if (g_persistentAudio->IsReady() && !IsRunning())
		{
			SetStartValue(g_persistentAudio->m_voicesVolumesCache[m_queue]);
			Start();
		}
	}

	/*void VoiceManager::TimeLerpAudioDecayStream::OnFlush() noexcept
	{
		__super::OnFlush();

		g_persistentAudio->InternalClearCurrentStream();
	}*/
#pragma endregion VoiceManager::TimeLerpAudioDecay
}