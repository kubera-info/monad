// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadSafety.h"
// Platform
#pragma comment(lib, "bcrypt.lib")
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Safety::SafetyCore
{
#pragma region SafetyManager
	inline BCRYPT_ALG_HANDLE SafetyManager::GetAlgorithmHandle(
		const wchar_t* algorithm,
		DWORD flags
	)
	{
		BCRYPT_ALG_HANDLE tmp = nullptr;
		THROW_EXC_NTERROR(BCryptOpenAlgorithmProvider(
			&tmp,
			algorithm,
			nullptr,
			flags), L"BCrypt Open Algorithm Provider")
			return tmp;
	}

	SafetyManager::SafetyManager(
		const wchar_t* algorithm,
		DWORD flags,
		LPCWCHAR length
	) :
		m_algorithm{ GetAlgorithmHandle(algorithm, flags) },
		m_property{ *this, length }
	{}

	SafetyManager::~SafetyManager()
	{
		BCryptCloseAlgorithmProvider(m_algorithm, 0);
	}
#pragma endregion

#pragma region SafetyManager::SafetyProperty
	SafetyManager::SafetyProperty::SafetyProperty(
		SafetyManager& algorithm, const wchar_t* pszProperty
	) :
		SafetyProperty{ InitSafetyProperty(algorithm, pszProperty) }
	{}

	uint32_t SafetyManager::SafetyProperty::InitSafetyProperty(
		SafetyManager& algorithm,
		const wchar_t* pszProperty
	)
	{
		DWORD cbData = 0, cbSize = 0;
		THROW_EXC_NTERROR(BCryptGetProperty(
			algorithm.m_algorithm,
			pszProperty,
			reinterpret_cast<PBYTE>(&cbSize),
			sizeof DWORD,
			&cbData,
			0), L"BCrypt Get Property");
		return cbSize;
	}

	SafetyManager::SafetyProperty::SafetyProperty(
		DWORD explicitSize
	) noexcept :
		m_size(explicitSize)
	{}

	PBYTE SafetyManager::SafetyProperty::Factory() const
	{
		auto pbBuffer = static_cast<PBYTE>(HeapAlloc(GetProcessHeap(), 0, m_size));
		if (nullptr == pbBuffer)
			THROW_EXC_GETLASTERROR(Exceptions::NullValue, L"Heap Alloc");
		return pbBuffer;
	}

	DWORD SafetyManager::SafetyProperty::Size() const noexcept
	{
		return m_size;
	}
#pragma endregion

#pragma region SafetyManager::SafetyPropertyInstance
	SafetyManager::SafetyPropertyInstance::SafetyPropertyInstance(
		const SafetyProperty& propFactory
	) :
		m_byteBuffer(propFactory.Factory())
	{}

	SafetyManager::SafetyPropertyInstance::~SafetyPropertyInstance()
	{
		HeapFree(GetProcessHeap(), 0, m_byteBuffer);
	}

	PBYTE SafetyManager::SafetyPropertyInstance::Data() const noexcept
	{
		return m_byteBuffer;
	}

	uint32_t SafetyManager::SafetyPropertyInstance::Size() const noexcept
	{
		return static_cast<uint32_t>(HeapSize(GetProcessHeap(), 0, m_byteBuffer));
	}
#pragma endregion
}