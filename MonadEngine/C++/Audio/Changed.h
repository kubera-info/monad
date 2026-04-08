// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <mmdeviceapi.h>
// Monad
#include "Kernel/ComManager.h"

namespace Monad::Audio
{
	struct PersistentAudio;
	/// <summary>
	/// Handles default audio device change events by invoking the audio subsystem.
	/// </summary>
	struct MMNotificationClient final : IMMNotificationClient
	{
		// Implements the IUnknown methods: AddRef, Release, and QueryInterface.
		MONAD_COM_REF_CNT(IMMNotificationClient);

	private:
		// Callback methods for device event notifications.
		HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
			EDataFlow flow,
			ERole role,
			LPCWSTR deviceID
		) final;
		HRESULT STDMETHODCALLTYPE OnDeviceAdded(
			LPCWSTR deviceID
		) final;
		HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR deviceID
		) final;
		HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(
			LPCWSTR deviceID,
			DWORD newState
		) final;
		HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
			LPCWSTR deviceID,
			const PROPERTYKEY key
		) final;
	};
}
