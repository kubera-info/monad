// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "State.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "Persistent.h"

using namespace std;

namespace Monad::Audio
{
	using namespace Exceptions;

	void XAudioState::OnProcessingPassEnd() noexcept
	{
	}

	void XAudioState::OnProcessingPassStart() noexcept
	{
	}

	void XAudioState::OnCriticalError(
		HRESULT
	) noexcept
	{
		g_persistentAudio->Restart();
	}

	void XAudioState::Mute(
		InitializerListQueues listOfQueues
	) noexcept
	{
		if (g_persistentAudio->IsReady())
			for (const auto& queue : listOfQueues)
				if(m_voicesManagers.contains(queue))
#if defined _DEBUG
					m_voicesManagers.at(queue).Mute();
#else
					m_voicesManagers.find(queue)->second.Mute();
#endif
	}

	void XAudioState::SetMasterVoiceVolume(
		const float volume
	) noexcept
	{
		m_masteringVoice->SetVolume(volume);
	}

#pragma region XAudioState
	HRESULT XAudioState::Initialize()
	{
		if (m_xAudio2)
			return E_FAIL;
		V_RETURN(XAudio2Create(
			&m_xAudio2,
			0)
		);
		V_RETURN(m_xAudio2->RegisterForCallbacks(this));
#if defined _DEBUG
		// To see the debug output, you need to view ETW logs for this application:
		// Go to Control Panel, Administrative Tools, Event Viewer.
		// View->Show Analytic and Debug Logs.
		// Applications and Services Logs / Microsoft / Windows / XAudio2.
		// Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK
		constexpr XAUDIO2_DEBUG_CONFIGURATION debug{ .TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS };
		m_xAudio2->SetDebugConfiguration(&debug, 0);
#endif 

		IXAudio2MasteringVoice* tmpMasteringVoice(nullptr);
		V_RETURN(m_xAudio2->CreateMasteringVoice(&tmpMasteringVoice, 0U, 0U, XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT));
		m_masteringVoice.reset(tmpMasteringVoice);

		for (const auto& queue : g_persistentAudio->m_voicesVolumesCache)
		{
			m_voicesManagers.try_emplace(
				queue.first,
				m_xAudio2.Get(),
				queue.first
			);
		}
		return S_OK;
	}

	XAudioState::XAudioState() noexcept :
		m_masteringVoice{ nullptr, DestroyVoice }
	{
	}
#pragma endregion XAudioState
}
