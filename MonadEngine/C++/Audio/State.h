// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <xaudio2.h>
// STD
#include <unordered_map>
// Monad
#include "Kernel/Del.h"
#include "VoiceManager.h"

using namespace std;

namespace Monad::Audio
{
	/// <summary>
	/// Class encapsulating an initialized XAudio2 instance.
	/// </summary>
	struct XAudioState final : private IXAudio2EngineCallback
	{
		XAudioState() noexcept;
		~XAudioState();
		OPER_DEL_NO_DEF_CTOR(XAudioState);
		HRESULT Initialize();
		template<typename Self>
		auto& operator [](
			this Self&& self,
			const string& queue
			) noexcept
		{
			return self.m_voicesManagers.find(queue)->second;
		}
		void Mute(InitializerListQueues list = {
			STREAM,
			JINGLE }
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
		Kernel::UnorderedMapString<VoiceManager> m_voicesManagers;
	};
}