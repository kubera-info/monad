// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Hash.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace fs = std::filesystem;

namespace Monad::Safety::SafetyCore
{
#pragma region SHA256Manager
	SHA256Manager::SHA256Manager() :
		SafetyManager{
		BCRYPT_SHA256_ALGORITHM,
		BCRYPT_HASH_REUSABLE_FLAG,
		BCRYPT_HASH_LENGTH
	}
	{}

	ArrayKey256Bits SHA256Manager::HashSource(
		InitializerListArgHash buffers
	)
	{
		struct SHA256 final
		{
			SHA256(
				const SHA256Manager& algorithm
			)
			{
				THROW_EXC_NTERROR(
					BCryptCreateHash(
						algorithm.m_algorithm,
						&m_hash,//&tmp,
						nullptr,
						0,
						nullptr,
						0,
						BCRYPT_HASH_REUSABLE_FLAG),
					L"BCrypt Create Hash"
				)
			}
			~SHA256()
			{
				//if (m_hash)
				BCryptDestroyHash(m_hash);
			}
			OPER_DEL(SHA256);
			/// <summary>
			/// Calculates using SHA256Manager, SHA256, for a list of buffers
			/// </summary>
			/// <param name="algorithm">SHA256 engine</param>
			/// <param name="buffers">List of buffers</param>
			/// <returns>Output hash value</returns>
			ArrayKey256Bits HashSource(
				const SHA256Manager& algorithm,
				InitializerListArgHash buffers
			)
			{
				for (const auto& buffer : buffers)
				{
					if (std::holds_alternative<PtrString>(buffer))
						BCryptHashData(*std::get<PtrString>(buffer));
					else if (std::holds_alternative<PtrWString>(buffer))
						BCryptHashData(*std::get<PtrWString>(buffer));
					else if (std::holds_alternative<PtrSpanBytes>(buffer))
						BCryptHashData(*std::get<PtrSpanBytes>(buffer));
					else if (std::holds_alternative<PtrPath>(buffer))
						BCryptHashData(*std::get<PtrPath>(buffer));
					else
						std::unreachable();
				}

				//calculate the length of the hash
				SafetyPropertyInstance pbHash(algorithm.m_property);

				//close the hash
				THROW_EXC_NTERROR(BCryptFinishHash(
					m_hash,
					pbHash.Data(),
					algorithm.m_property.Size(),
					0),
					L"BCrypt Finish Hash");

				ArrayKey256Bits res{};
				assert(res.size() == algorithm.m_property.Size());
				memcpy(res.data(), pbHash.Data(), algorithm.m_property.Size());

				return res;
			}

		private:
			inline void BCryptHashDataGeneric(
				const void* data,
				size_t length,
				size_t blockSize
			)
			{
				THROW_EXC_NTERROR(::BCryptHashData(
					m_hash,
					const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data)),
					static_cast<DWORD>(length * blockSize),
					0),
					L"BCrypt Hash Data")
			}

			void BCryptHashData(
				const std::wstring hashMe
			)
			{
				BCryptHashDataGeneric(
					hashMe.data(),
					hashMe.size(),
					sizeof std::wstring::value_type);
			}

			void BCryptHashData(
				const std::string hashMe
			)
			{
				BCryptHashDataGeneric(
					hashMe.data(),
					hashMe.size(),
					sizeof std::string::value_type);
			}

			void BCryptHashData(
				const Kernel::SpanBytes& hashMe
			)
			{
				BCryptHashDataGeneric(
					hashMe.data(),
					hashMe.size(),
					sizeof Kernel::SpanBytes::value_type);
			}

			void BCryptHash(
				const fs::path& hashMe
			)
			{
				BCryptHashData(
					hashMe.wstring()
				);
			}

			BCRYPT_HASH_HANDLE m_hash = nullptr;
		};

		thread_local SHA256 pLocal(*this);
		return pLocal.HashSource(*this, buffers);
	}
#pragma endregion
}