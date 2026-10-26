// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <bcrypt.h>
#include <Windows.h>

// STD
#include <array>
#include <atomic>

namespace Monad
{
	namespace Safety
	{
		constexpr size_t g_keySizeBytes = 32;
		using ArrayKey256Bits = std::array<uint8_t, g_keySizeBytes>;

		namespace SafetyCore
		{
			interface SafetyManager abstract
			{
			protected:
				SafetyManager(
					const wchar_t* algorithm,
					DWORD flags,
					LPCWCHAR length
				);
				~SafetyManager();
				SafetyManager() = delete;
				SafetyManager(const SafetyManager&) = delete;
				SafetyManager& operator=(const SafetyManager&) = delete;
				SafetyManager(SafetyManager&&) = delete;
				SafetyManager& operator=(SafetyManager&&) = delete;
					inline BCRYPT_ALG_HANDLE GetAlgorithmHandle(
						const wchar_t* algorithm,
						DWORD flags
					);

				const BCRYPT_ALG_HANDLE m_algorithm;

				struct SafetyProperty final
				{
					SafetyProperty(
						SafetyManager& algorithm,
						const wchar_t* property
					);
					SafetyProperty(
						DWORD explicitSize = 0
					) noexcept;
					PBYTE Factory() const;
					DWORD Size() const noexcept;

				private:
					uint32_t InitSafetyProperty(
						SafetyManager& algorithm,
						const wchar_t* pszProperty
					);

					uint32_t m_size;
				} const m_property;

				struct SafetyPropertyInstance final
				{
					SafetyPropertyInstance(
						const SafetyProperty& propFactory
					);
					~SafetyPropertyInstance();
					SafetyPropertyInstance() = delete;
					SafetyPropertyInstance(const SafetyPropertyInstance&) = delete;
					SafetyPropertyInstance& operator=(const SafetyPropertyInstance&) = delete;
					SafetyPropertyInstance(SafetyPropertyInstance&&) = delete;
					SafetyPropertyInstance& operator=(SafetyPropertyInstance&&) = delete;
					PBYTE Data() const noexcept;
					uint32_t Size() const noexcept;

				private:
					PBYTE m_byteBuffer;
				};
			};
		}
	}
	namespace Files
	{
		struct VectorBytesArchive;
	}
}