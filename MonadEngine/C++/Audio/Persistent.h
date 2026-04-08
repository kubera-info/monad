// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"
// Monad
#include <Kernel/Templates.h>
#include "__MonadAudio.h"
#include "Kernel/Patterns.h"
#include "Changed.h"
#include "AudioThread.h"
#include "MediaStreamer.h"
#include "WaveWithCallback.h"

namespace Monad::Audio
{
	struct VoiceManager;
	struct XAudioState;

	/// <summary>
	/// Audio core independent of device state.
	/// </summary>
	struct PersistentAudio final
	{
		friend TaskStartStream;
		friend TaskRestart;
		friend VoiceManager;
		friend XAudioState;

		/// <summary>
		/// CTOR
		/// </summary>
		/// <param name="config">A reference to a flag indicating whether all sounds are enabled.</param>
		PersistentAudio(
			const bool& config,
			InitializerListQueues queues
		);
		~PersistentAudio();
		OPER_DEL(PersistentAudio);
		/// <summary>
		/// Parses monad's custom audio format, and creates object ready for playing in XAudio2
		/// </summary>
		/// <param name="filename">Custom audio file data name.</param>
		/// <param name="blob">Binary blob containing a file in a custom audio format.</param>
		void CreateSound(
			const std::string& filename,
			const Kernel::VectorBytes& blob
		);
		/// <summary>
		/// Audio buffer containing PCM data in WAVEFORMATEX format, suitable for XAudio2 playback.
		/// </summary>
		/// <param name="waveFornatEx">structure WAVEFORMATEX</param>
		/// <param name="buffer">Raw audio data buffer.</param>
		/// <returns>The sound ready for playing</returns>
		PtrWaveBuffer GetPreparedSound(
			const WAVEFORMATEX* const waveFornatEx,
			Kernel::VectorBytes&& buffer
		);
		/// <summary>
		/// PCM buffer in WAVEFORMATEX format, ready for playback in XAudio2.
		/// </summary>
		/// <param name="filename">Name of the sound</param>
		/// <param name="waveFornatEx">structure WAVEFORMATEX</param>
		/// <param name="buffer">Raw audio data buffer.</param>
		void TryEmplacePreparedSound(
			const std::string& filename,
			const WAVEFORMATEX* const waveFornatEx,
			Kernel::VectorBytes&& buffer
		);
		/// <summary>
		/// PCM buffer with WAVEFORMATEX ready for playing in XAudio2, always replaces the sound.
		/// </summary>
		/// <param name="filename">Name of the sound.</param>
		/// <param name="waveFornatEx">structure WAVEFORMATEX<./param>
		/// <param name="buffer">Raw audio data buffer.</param>
		void InsertOrAssignPreparedSound(
			const std::string& filename,
			const WAVEFORMATEX* const waveFornatEx,
			Kernel::VectorBytes&& buffer
		);
		/// <summary>
		///Converts an MP3 (or other audio format) into a WAV block and a WAVEFORMATEX* structure.
		/// </summary>
		/// <param name="filename">Name of stream</param>
		/// <param name="stream">Entire MP3 file provided as a stream.</param>
		void CreateStream(
			const std::string& filename,
			IStream* stream
		);
		/// <summary>
		/// Applies the same volume level to every voice in the audio queue.
		/// </summary>
		/// <param name="volume">Voice volume: 0.0f .. 1.0f</param>
		/// <param name="queue">Queue ID</param>
		void SetVoiceVolumeGeneric(
			const float volume,
			const std::string& queue
		);
		/// <summary>
		/// Stops playback of the queue and flushes all its contents.
		/// </summary>
		/// <param name="queue">Queue ID</param>
		void ResetQueue(
			const std::string& queue
		);
		/// <summary>
		/// Restores the state to what it was before playing DESCRIBE.
		/// No state changes occur when DESCRIBE is inactive.
		/// </summary>			
		void ResetQueueDescribe();
		/// <summary>
		/// Plays one buffer
		/// </summary>
		/// <param name="filename">Identifier for the PCM audio buffer.</param>
		/// <param name="queue">Queue ID</param>
		/// <param name="callback">The callback of PCM Buffer</param>
		void EnqueueSound(
			const std::string& filename,
			const std::string& queue,
			IWaveCallback* const callback = nullptr
		);
		/// <summary>
		/// Flushes the Voice Manager for jingles and immediately starts playback of a new sound.
		/// </summary>
		/// <param name="filename">The name of PCM Buffer.</param>
		void PlayJingle(
			const std::string& filename
		);
		/// <summary>
		/// Flushes the Voice Manager for DESCRIBE and immediately starts playback of a new sound.
		/// </summary>
		/// <param name="filename">The name of PCM Buffer.</param>
		void Describe(
			const std::string& filename
		);
		/// <summary>
		/// Determines if the Voice Manager has any active playback.
		/// </summary>
		/// <param name="queue">Queue ID</param>
		/// <returns>Returns true if a voice is currently playing, including when there is a buffer queued for playback.</returns>
		bool IsPlaying(
			const std::string& queue = STREAM
		) noexcept;
		/// <summary>
		/// Disables all queues except for DESCRIBE; the help speech queue remains always available.
		/// </summary>
		void SetMuted();
		/// <summary>
		/// Checks whether the audio subsystem is ready and enabled for playback.
		/// </summary>
		/// <returns>true if audio subsystem is ready and enabled./returns>
		inline bool IsReady() const noexcept;
		/// <summary>
		/// Closes and deinitializes the audio subsystem.
		/// </summary>
		void Shutdown();
		/// <summary>
		/// Restart audio subsystem after invalidation.
		/// </summary>
		void Restart();
		/// <summary>
		/// Stops the audio subsystem in response to a critical error.
		/// </summary>
		//void ShutdownOnError() noexcept;

		/// <summary>
		/// Starts playback of a randomly selected stream from the list of all available streams.
		/// </summary>
		void SetStream();
		/// <summary>
		/// Silences and stops all audio queues, except for the DESCRIBE queue, which remains active.
		/// </summary>
		void Mute();
		/// <summary>
		/// Reduces the volume of the stream when the help speech is playing.
		/// </summary>
		void OnDescVolumes();
		/// <summary>
		/// Retrieves the volume level of the queue.
		/// </summary>
		/// <param name="queue">Queue ID</param>
		/// <returns>Queue volume, maintained as a persistent state even across XAudio invalidations.</returns>
		float GetVolume(
			const std::string& queue
		) const noexcept;

	private:
		const Kernel::Me<PersistentAudio> c_me;
		Kernel::UnorderedMapString<float> m_voicesVolumesCache;
		ATG::CriticalSectionLockable m_lockAudio;
		std::atomic_bool m_inRestart = false;

		void PlayImmediate(
			const std::string& filename,
			const std::string& queue,
			IWaveCallback* const callback = nullptr
		);
		const bool IsEnabled() const noexcept;
		void InternalSetStream();

		void InsertTaskStream(TaskAudioGeneric* const task);
		PtrWaveBuffer GetSound(const std::string& filename);

		TaskRestart m_taskRestart;
		TaskShutdown m_taskShutdown;
		TaskStartStream m_taskStartStream;
		/// <summary>
		/// Determines if audio playback is permitted according to the application configuration.
		/// </summary>
		const bool& m_config;

		/// <summary>
		/// List of identifiers for all currently loaded audio streams.
		/// </summary>
		Kernel::VectorRandomString m_streamsNames;
		/// <summary>
		/// Media Foundation Manager.
		/// </summary>
		MFManager m_mfManager;
		/// <summary>
		/// A unique repository of WAVEFORMATEX structures for all audio formats discovered in assets.
		/// </summary>
		SetWaveFormatEx m_formatSignatures;
		Kernel::UnorderedMapString<PtrWaveBuffer> m_waveBuffers;
		Kernel::UnorderedMapString<MediaStreamer> m_streams;
		struct StreamWave final : IWaveCallback
		{
			using IWaveCallback::IWaveCallback;

			void OnAfterWave() noexcept final;
		} m_nextWave;
		struct StopDescribe final : IWaveCallback
		{
			using IWaveCallback::IWaveCallback;

			void OnAfterWave() noexcept final;
		} m_stopDescribe;
		Threads::ThreadPool m_threadPoolStreams;
#pragma region Unsafe calls
		void InternalRestart();
		void InternalInit();
		bool InternalIsPlaying(
			const std::string& queue
		) const noexcept;
		void InternalShutdown() noexcept;
		void InternalSetVoiceVolume(
			const float volume,
			const std::string& queue
		);
		void InternalSetVoiceVolumeXAudio2(
			const float volume,
			const std::string& queue
		);
		void InternalResetQueue(
			const std::string& queue
		);
		void InternalEnqueueSound(
			const std::string& filename,
			const std::string& queue = STREAM,
			IWaveCallback* const callback = nullptr
		);
		void InternalEnqueueStream(
			const std::string& filename
		);
		void InternalAfterWave(
			const std::string& queue
		);
		void InternalSetMuted();
		void InternalClearCurrentStream();
		void InternalStartStream();
#pragma endregion

		std::unique_ptr<XAudioState> m_xAudioState;
		Microsoft::WRL::ComPtr<IMMDeviceEnumerator> m_enumerator;
		MMNotificationClient m_clientAuto;
	} extern* g_persistentAudio;
}
