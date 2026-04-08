// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Cipher.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "Files/Struct.h"
#include "Globals/__MonadGlobals.h"

using namespace std;

namespace fs = std::filesystem;

namespace Monad
{
	using namespace Files;
	using namespace Kernel;

	namespace Safety
	{
		namespace SafetyCore
		{
			namespace
			{
				wstring g_cbc(
					BCRYPT_CHAIN_MODE_CBC,
					_countof(BCRYPT_CHAIN_MODE_CBC)
				);
				constexpr size_t g_ivSize = 16;
				using ArrayIV = array<uint8_t, g_ivSize>;

				constexpr ArrayIV AES_256_IV{ 0xFB, 0x56, 0xA7, 0x16, 0xB4, 0x3E, 0x16, 0xA9, 0x39, 0x74, 0x54, 0x18, 0x2D, 0x49, 0x18, 0x2D };
				constexpr DWORD TYPE_SIZE = sizeof FOURCC;

#pragma region KeyHandle
				struct KeyHandle final
				{
					KeyHandle() = default;
					~KeyHandle()
					{
						if (m_key)
							BCRYPT_SUCCESS(
								BCryptDestroyKey(
									m_key)
							);
						/*if (!BCRYPT_SUCCESS(BCryptDestroyKey(m_key)))
						terminate();*/
					}
					OPER_DEL_NO_DEF_CTOR(KeyHandle);

					BCRYPT_KEY_HANDLE m_key{};
				};
			}
#pragma endregion

#pragma region AES256Manager
			AES256Manager::AES256Manager() :
				SafetyManager{
				BCRYPT_AES_ALGORITHM,
				0,
				BCRYPT_BLOCK_LENGTH
			}
			{
				THROW_EXC_NTERROR(BCryptSetProperty(
					m_algorithm,
					BCRYPT_CHAINING_MODE,
					reinterpret_cast<PUCHAR>(g_cbc.data()),
					sizeof BCRYPT_CHAIN_MODE_CBC,
					0), L"BCrypt Set Property");
			}
#define DATA (&source[TYPE_SIZE])
#define SIZE (static_cast<DWORD>(source.size() - TYPE_SIZE))
#define	CRYPT_INIT auto fileKey = sha.HashSource({ &key, &Globals::g_sha256Salt });\
				assert(m_property.Size() == AES_256_IV.size());\
				KeyHandle keyHandle;\
				DWORD cbCipherText = 0, cbData = 0;\
				SafetyPropertyInstance pbIV(m_property);\
				memcpy(pbIV.Data(), AES_256_IV.data(), m_property.Size());\
				THROW_EXC_NTERROR(BCryptGenerateSymmetricKey(\
				m_algorithm,\
				&keyHandle.m_key,\
				nullptr,\
				0,\
				fileKey.data(),\
				static_cast<DWORD>(fileKey.size()),\
				0), L"Hash Source");

			void AES256Manager::GetClear(
				SHA256Manager& sha,
				const fs::path& key,
				VectorBytesArchive& source
			)
			{
				CRYPT_INIT;

				THROW_EXC_NTERROR(BCryptDecrypt(
					keyHandle.m_key,
					DATA,
					SIZE,
					nullptr,
					pbIV.Data(),
					m_property.Size(),
					nullptr,
					0,
					&cbCipherText,
					BCRYPT_BLOCK_PADDING), L"BCrypt Decrypt");
				SafetyPropertyInstance pbCipherText(cbCipherText);
				THROW_EXC_NTERROR(BCryptDecrypt(
					keyHandle.m_key,
					DATA,
					SIZE,
					nullptr,
					pbIV.Data(),
					m_property.Size(),
					DATA,
					SIZE,
					&cbData,
					BCRYPT_BLOCK_PADDING), L"BCrypt Decrypt");
				source.resize(cbData + TYPE_SIZE);
			}
			void AES256Manager::GetEncrypted(
				SHA256Manager& sha,
				const fs::path& key,
				VectorBytesArchive& source
			)
			{
				CRYPT_INIT;

				THROW_EXC_NTERROR(BCryptEncrypt(
					keyHandle.m_key,
					DATA,
					SIZE,
					nullptr,
					pbIV.Data(),
					m_property.Size(),
					nullptr,
					0,
					&cbCipherText,
					BCRYPT_BLOCK_PADDING),
					L"BCrypt Encrypt"
				);
				const DWORD SizeOfPlainText = SIZE;
				source.resize(cbCipherText + TYPE_SIZE);
				THROW_EXC_NTERROR(BCryptEncrypt(
					keyHandle.m_key,
					DATA,
					SizeOfPlainText,
					nullptr,
					pbIV.Data(),
					m_property.Size(),
					DATA,
					cbCipherText,
					&cbData,
					BCRYPT_BLOCK_PADDING),
					L"BCrypt Encrypt"
				);
			}
		}
#pragma endregion
	}
}