// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "WaveWithCallback.h"

namespace Monad::Audio
{
	WaveToPlayWithCallback::WaveToPlayWithCallback() noexcept :
		m_callback{ nullptr },
		m_ifEnqueAndInvalidate{ true }
	{}

	WaveToPlayWithCallback::WaveToPlayWithCallback(
		const std::string& filename,
		IWaveCallback* const callback,
		const PtrWaveBuffer& waveBuffer
	) :
		m_filename{ filename },
		m_callback{ callback },
		m_waveBuffer{ waveBuffer },
		m_ifEnqueAndInvalidate{ true }
	{}

	bool WaveToPlayWithCallback::IsCallback() const noexcept
	{
		return nullptr != m_callback;
	}

	const std::string& WaveToPlayWithCallback::GetFilename() const noexcept
	{
		return m_filename;
	}

	bool WaveToPlayWithCallback::empty() const noexcept
	{
		return m_filename.empty();
	}

	void WaveToPlayWithCallback::OnAfterWave() noexcept
	{
		InvalidatePlayOption();
		if (IsCallback())
			m_callback->OnAfterWave();
	}

	const PtrWaveBuffer& WaveToPlayWithCallback::data() const noexcept
	{
		return m_waveBuffer;
	}

	bool WaveToPlayWithCallback::IfPlay() const noexcept
	{
		return m_ifEnqueAndInvalidate;
	}

	void WaveToPlayWithCallback::InvalidatePlayOption() noexcept
	{
		m_ifEnqueAndInvalidate = false;
	}

	bool WaveToPlayWithCallback::IsCompatible(const WaveToPlayWithCallback& toCompare) const noexcept
	{
		if (empty() || toCompare.empty())
			return false;
		return m_waveBuffer->c_waveFornatEx == toCompare.m_waveBuffer->c_waveFornatEx;
	}
}
