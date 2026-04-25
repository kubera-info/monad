// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Persistent.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "State.h"
#include "Jingle.h"
#include "Threads/__MonadThread.h"

using namespace std;

namespace Monad
{
	using namespace Exceptions;
	using namespace Kernel;

	namespace Audio
	{
#pragma region PersistentAudio
		PersistentAudio::PersistentAudio(
			const bool& config,
			InitializerListQueues queues
		) :
			c_me(this, g_persistentAudio),
			m_config{ config },
			m_threadPoolStreams{ Threads::THREAD_NAME_AUDIO_STREAM_POOL },
			m_voicesVolumesCache{
				ranges::to<Kernel::FlatMapString<float>>(
					views::transform
					(
						queues,
						[](const std::string& queue)
						{
							return std::make_pair(queue, VOLUME_MAX);
						}
					)
				)
			}
		{
			THROW_EXC_AUDIO(CoCreateInstance(
				__uuidof(MMDeviceEnumerator),
				nullptr,
				CLSCTX_ALL,
				__uuidof(IMMDeviceEnumerator),
				reinterpret_cast<void**>(m_enumerator.GetAddressOf())),
				L"Enum");
			THROW_EXC_AUDIO(m_enumerator->RegisterEndpointNotificationCallback(&m_clientAuto), L"Reg #2");
			InternalInit();
		}

		void PersistentAudio::Shutdown()
		{
			const lock_guard lg(m_lockAudio);
			InternalShutdown();
		}

		void PersistentAudio::Restart()
		{
			InsertTaskStream(&m_taskRestart);
		}

		/*void PersistentAudio::ShutdownOnError() noexcept
		{
			m_inRestart = true;
			InsertTaskStream(&m_taskShutdown);
		}*/

		inline void PersistentAudio::InsertTaskStream(
			TaskAudioGeneric* const task
		)
		{
			m_threadPoolStreams.InsertTask(task);
		}

		/*inline bool PersistentAudio::InternalSoundExists(const wstring& filename) const
		{
			return m_buffers.contains(filename);
		}*/

		bool PersistentAudio::InternalIsPlaying(
			const string& queue
		) const noexcept
		{
			if (IsReady())
				return m_xAudioState->operator[](queue).IsPlaying();
			else
				return false;
		}

		PersistentAudio::~PersistentAudio()
		{
			m_enumerator->UnregisterEndpointNotificationCallback(&m_clientAuto);
			Shutdown(); // Must be locked (implicit dtor is not enough)
		}

		void PersistentAudio::CreateSound(
			const string& filename,
			const VectorBytes& blob
		)
		{
			VectorBytes newBuf(
				&blob[sizeof WAVEFORMATEX],
				blob.data() + blob.size()
			);
			const lock_guard lg(m_lockAudio);
			InsertOrAssignPreparedSound(
				filename,
				reinterpret_cast<const WAVEFORMATEX*>(blob.data()),
				move(newBuf)
			);
		}

		PtrWaveBuffer PersistentAudio::GetPreparedSound(
			const WAVEFORMATEX* const waveFornatEx, VectorBytes&& buffer
		)
		{
			PtrWaveBuffer newReferenceWaveBuffer;
			newReferenceWaveBuffer.Attach(new WaveBuffer(
				*waveFornatEx,
				move(buffer),
				m_formatSignatures)
			);
			return newReferenceWaveBuffer;
		}

		void PersistentAudio::TryEmplacePreparedSound(
			const string& filename,
			const WAVEFORMATEX* const waveFornatEx,
			VectorBytes&& buffer
		)
		{
			m_waveBuffers.try_emplace(
				filename,
				GetPreparedSound(
					waveFornatEx,
					move(buffer))
			);
		}

		void PersistentAudio::InsertOrAssignPreparedSound(
			const string& filename,
			const WAVEFORMATEX* const waveFornatEx,
			VectorBytes&& buffer
		)
		{
			m_waveBuffers.insert_or_assign(
				filename,
				GetPreparedSound(
					waveFornatEx,
					move(buffer))
			);
		}

		PtrWaveBuffer PersistentAudio::GetSound(
			const string& filename
		)
		{
			if (const auto waveBuffer(m_waveBuffers.find(filename)); m_waveBuffers.cend() != waveBuffer)
				return waveBuffer->second;
			else
				return nullptr;
		}

		void PersistentAudio::CreateStream(
			const string& filename,
			IStream* const stream
		)
		{
			const lock_guard lg(m_lockAudio);
			m_streams.emplace(filename, stream);
			m_streamsNames.emplace_back(filename);
		}

		void PersistentAudio::SetMasterVoiceVolume(
			const float volume
		)
		{
			const lock_guard lg(m_lockAudio);
			if (IsReady())
				m_xAudioState->SetMasterVoiceVolume(volume);
		}

		void PersistentAudio::SetVoiceVolumeGeneric(
			const float volume,
			const string& queue
		)
		{
			const lock_guard lg(m_lockAudio);
			if (IsReady() && g_persistentAudio->InternalContainsQueue(STREAM))
				m_xAudioState->operator[](queue).SetVoiceVolumeGeneric(volume);
		}

		void PersistentAudio::ResetQueue(
			const string& queue
		)
		{
			const lock_guard lg(m_lockAudio);
			InternalResetQueue(queue);
		}

		void PersistentAudio::ResetQueueDescribe()
		{
			const lock_guard lg(m_lockAudio);
			InternalResetQueue(DESCRIBE);
			OnDescVolumes();
		}

		void PersistentAudio::EnqueueSound(
			const string& filename,
			const string& queue,
			IWaveCallback* const callback
		)
		{
			const lock_guard lg(m_lockAudio);
			InternalEnqueueSound(
				filename,
				queue,
				callback);
		}

		void PersistentAudio::PlayImmediate(
			const string& filename,
			const string& queue,
			IWaveCallback* const callback
		)
		{
			const lock_guard lg(m_lockAudio);
			if (IsReady())
			{
				InternalResetQueue(queue);
				InternalEnqueueSound(filename, queue, callback);
			}
		}

		const bool PersistentAudio::IsEnabled() const noexcept
		{
			return m_config;
		}

		void PersistentAudio::PlayJingle(
			const string& filename
		)
		{
			if (IsEnabled())
				PlayImmediate(
					filename,
					JINGLE
				);
		}

		void PersistentAudio::Describe(
			const string& filename
		)
		{
			SetVoiceVolumeGeneric(
				VOLUME_SILENT,
				STREAM
			);
			PlayImmediate(
				filename,
				DESCRIBE,
				&m_stopDescribe
			);
		}

		void PersistentAudio::InternalAfterWave(
			const string& queue
		)
		{
#if MONAD_HARDWARE_EXCEPTIONS == 1
			__try
			{
#endif		
				if (IsReady())
					m_xAudioState->operator[](queue).OnAfterWave();
#if MONAD_HARDWARE_EXCEPTIONS == 1
			}
			__except (Exceptions::filter(
				GetExceptionCode(),
				GetExceptionInformation())
				)
			{
			}
#endif
		}

		void PersistentAudio::InternalInit()
		{
			m_inRestart = true;
			/*size_t SleepMe(1);
			while (1024 > SleepMe && !IsDefaultAudioDevice())
			{
				Sleep(SleepMe);
				SleepMe <<= 1;
			}*/
			m_xAudioState = make_unique<XAudioState>();
			if (FAILED(m_xAudioState->Initialize()))
				m_xAudioState.reset();
			m_inRestart = false;
		}

		void PersistentAudio::InternalShutdown() noexcept
		{
			m_xAudioState.reset();
			m_inRestart = false;
		}

		/*void PersistentAudio::InternalSetMasterVolume(float volume)
		{
			m_masterVolume = volume;
			if (IsReady())
				m_xAudioState->SetMasterVolume(volume);
		}*/

		void PersistentAudio::InternalSetVoiceVolume(
			const float volume,
			const string& queue
		)
		{
			if (InternalContainsQueue(queue))
			{
				m_voicesVolumesCache[queue] = volume;
				InternalSetVoiceVolumeXAudio2(
					volume,
					queue);
			}
		}

		void PersistentAudio::InternalSetVoiceVolumeXAudio2(
			const float volume,
			const string& queue
		)
		{
			if (IsReady() && InternalContainsQueue(queue))
				m_xAudioState->operator[](queue).SetVoiceVolume(volume);
		}

		bool PersistentAudio::InternalContainsQueue(
			const string& queue
		) const noexcept
		{
			return m_voicesVolumesCache.contains(queue);
		}

		void PersistentAudio::InternalResetQueue(
			const string& queue
		)
		{
			if (IsReady() && InternalContainsQueue(queue))
				m_xAudioState->operator[](queue).ResetManager();
		}

		void PersistentAudio::InternalEnqueueSound(
			const string& filename,
			const string& queue,
			IWaveCallback* const callback
		)
		{
			auto snd = GetSound(filename);
			if (!snd)
				return;
			if (IsReady() && InternalContainsQueue(queue))
				m_xAudioState->operator[](queue).AudioEnqueueSound({
				filename,
				callback,
				snd });
		}

		void PersistentAudio::InternalEnqueueStream(
			const string& filename
		)
		{
			InternalEnqueueSound(
				filename,
				STREAM,
				&m_nextWave);
		}

		void PersistentAudio::InternalRestart()
		{
			InternalInit();
			InternalSetStream();
		}

		bool PersistentAudio::IsPlaying(
			const string& queue
		) noexcept
		{
			if (IsReady() && InternalContainsQueue(queue))
				return InternalIsPlaying(queue);
			else
				return false;
		}

		void PersistentAudio::StreamWave::OnAfterWave() noexcept
		{
			try
			{
				g_persistentAudio->InternalSetStream();
			}
			MONAD_CATCH;
		}

		void PersistentAudio::StopDescribe::OnAfterWave() noexcept
		{
			g_persistentAudio->SetVoiceVolumeGeneric(
				VOLUME_MAX,
				STREAM
			);
		}

		void PersistentAudio::InternalClearCurrentStream()
		{
			erase_if(
				m_waveBuffers,
				[&](const auto& buffer)
				{
					return ranges::find(
						m_streamsNames,
						buffer.first) != m_streamsNames.cend();
				}
			);
		}

		void PersistentAudio::InternalStartStream()
		{
			if (!m_xAudioState || m_streamsNames.empty())
				return;
			OnDescVolumes();
			auto& audioState = m_xAudioState->operator[](STREAM);
			audioState.ResetManager();
			if (IsReady() && IsEnabled() && !audioState.m_isQueueErased)
			{
				const auto& mediaStreamer = m_streams.find(
					m_streamsNames.GetRandomString());
				THROW_EXC_ONEND(
					m_streams,
					mediaStreamer,
					L"Find Stream");
				auto newWaveBuf = mediaStreamer->second.ReadAll();
				if (IsReady() && IsEnabled() && !audioState.m_isQueueErased)
				{
					const lock_guard lg(m_lockAudio);
					InternalClearCurrentStream();
					TryEmplacePreparedSound(
						mediaStreamer->first,
						&mediaStreamer->second.GetOutputWaveFormatEx(),
						move(newWaveBuf));
					InternalEnqueueStream(mediaStreamer->first);
				}
			}
		}

		void PersistentAudio::SetStream()
		{
			const lock_guard lg(m_lockAudio);
			InternalSetStream();
		}

		void PersistentAudio::Mute()
		{
			const lock_guard lg(m_lockAudio);
			if (IsReady())
				m_xAudioState->Mute();
		}

		void PersistentAudio::OnDescVolumes()
		{
			if (IsReady() && g_persistentAudio->InternalContainsQueue(STREAM))
				m_xAudioState->operator[](STREAM)
				.SetVoiceVolumeGeneric(
					m_xAudioState->operator[](DESCRIBE).IsPlaying()
					? VOLUME_SILENT
					: VOLUME_MAX);
		}

		float PersistentAudio::GetVolume(
			const string& queue
		) const noexcept
		{
#if defined _DEBUG
			return m_voicesVolumesCache.at(queue);
#else
			return m_voicesVolumesCache.find(queue)->second;
#endif
		}

		/*bool PersistentAudio::IsDefaultAudioDevice() noexcept
		{
			IMMDevice* IMMD = nullptr;
			return S_OK == m_enumerator->
				GetDefaultAudioEndpoint(
					eRender,
					eConsole,
					&IMMD);
		}*/

		void PersistentAudio::InternalSetStream()
		{
			if (IsEnabled())
				InsertTaskStream(&m_taskStartStream);
		}

		void PersistentAudio::SetMuted()
		{
			const lock_guard lg(m_lockAudio);
			InternalSetMuted();
		}

		inline bool PersistentAudio::IsReady() const noexcept
		{
			return !InAnyCloseReason() && m_xAudioState && !m_inRestart;
		}

		void PersistentAudio::InternalSetMuted()
		{
			if (IsReady())
				if (IsEnabled())
				{
					InternalSetVoiceVolume(
						VOLUME_MAX,
						STREAM);
					InternalSetVoiceVolume(
						VOLUME_MAX,
						JINGLE);
					InternalSetStream();
				}
				else
					m_xAudioState->Mute();
		}
#pragma endregion

		PersistentAudio* g_persistentAudio = nullptr;
	}
}