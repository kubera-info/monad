// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <xaudio2.h>

// STD
#include <map>

// Monad
#include "VoiceManager.h"
#include "Exceptions/Exceptions.h"

using namespace std;

namespace Monad::Audio
{
	/// <summary>
	/// Class encapsulating an initialized XAudio2 instance.
	/// </summary>
	struct XAudioState final : private IXAudio2EngineCallback
	{
		XAudioState() noexcept;
		XAudioState(const XAudioState&) = delete;
		XAudioState& operator=(const XAudioState&) = delete;
		XAudioState(XAudioState&&) = delete;
		XAudioState& operator=(XAudioState&&) = delete;
		HRESULT Initialize();
		template<typename Self>
		auto& operator [](
			this Self&& self,
			const Queue& queue
			)
		{
			auto itVoiceManager = self.m_voicesManagers.find(queue);
			THROW_EXC_ONEND(self.m_voicesManagers, itVoiceManager, L"VoiceManager");
			return itVoiceManager->second;
		}
		void Mute(InitializerListQueues list = {
			STREAM,
			JINGLE }
			) noexcept;
		void SetMasterVoiceVolume(
			const float volume
		) noexcept;

	private:
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnProcessingPassEnd() final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnProcessingPassStart() final;
		COM_DECLSPEC_NOTHROW void STDMETHODCALLTYPE OnCriticalError(
			HRESULT Error
		) final;

		Microsoft::WRL::ComPtr<IXAudio2> m_xAudio2;
		std::unique_ptr<IXAudio2MasteringVoice, FnDestroyVoice> m_masteringVoice;

		/// <summary>
		/// Structure VoiceManagers aggregates all virtual voices for management and playback.
		/// </summary>
		using UnorderedMapVoiceManager = map<Queue, VoiceManager>;
		UnorderedMapVoiceManager m_voicesManagers;
	};
}