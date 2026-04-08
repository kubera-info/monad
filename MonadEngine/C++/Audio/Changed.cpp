// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Changed.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "Persistent.h"

namespace Monad::Audio
{
	HRESULT STDMETHODCALLTYPE MMNotificationClient::OnDefaultDeviceChanged(
		EDataFlow,
		ERole,
		LPCWSTR
	)
	{
		//if ((eRender == flow || eAll == flow) && eConsole == role /* && !Audio::g_persistentAudio->m_xAudioState */)
		try
		{
			g_persistentAudio->Restart();
		}
		MONAD_CATCH;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MMNotificationClient::OnDeviceAdded(
		const wchar_t*
	)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MMNotificationClient::OnDeviceRemoved(
		const wchar_t*
	)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MMNotificationClient::OnDeviceStateChanged(
		const wchar_t*,
		DWORD
	)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE MMNotificationClient::OnPropertyValueChanged(
		const wchar_t*,
		const PROPERTYKEY
	)
	{
		return S_OK;
	}
}