// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Struct.h"
// STD
#include <mutex>
// Monad
#include "__MonadFile.h"
#include "Exceptions/Exceptions.h"
#include "Kernel/Templates.h"
// Library
#include "Modules/7zip/C/Alloc.h"
#include "Modules/7zip/C/Lzma2Dec.h"
#include "Modules/7zip/C/Lzma2Enc.h"

using namespace std;
namespace fs = std::filesystem;

namespace Monad
{
	using namespace Kernel;

	namespace Files
	{
		namespace
		{
			HRESULT SResToHRESULT(
				SRes res
			) noexcept
			{
				switch (res)
				{
				case SZ_OK: return S_OK;
				case SZ_ERROR_MEM: return E_OUTOFMEMORY;
				case SZ_ERROR_PARAM: return E_INVALIDARG;
					// case SZ_ERROR_PROGRESS: return E_ABORT;
				case SZ_ERROR_DATA: return E_UNEXPECTED;
				default: return E_FAIL;
				}
			}
		}

		bool VectorBytesArchive::Hash(
			Safety::SafetyState& cipher,
			const fs::path& key
		)
		{
			return CalcHash(
				cipher,
				key
			) == GetHeader().m_checksum;
		}

		inline void VectorBytesArchive::Resize(
			size_t newSize
		)
		{
			resize(newSize + sizeof FileMeta);
		}

		inline void VectorBytesArchive::ResizeWithPadding(
			size_t newSize
		)
		{
			Resize(newSize + BCRYPT_OBJECT_ALIGNMENT);
			shrink_to_fit();
			Resize(newSize);
		}

		inline Safety::ArrayKey256Bits VectorBytesArchive::CalcHash(
			Safety::SafetyState& cipher,
			const fs::path& key
		)
		{
			const SpanBytes blob2Hash{
				data() + offsetof(FileMeta, m_sizeOfArchive),
				GetHeader().m_sizeOfArchive + sizeof FileMeta - sizeof FOURCC - sizeof Safety::ArrayKey256Bits };
			return cipher.HashSource({
				&key,
				&blob2Hash }
				);
		}

		inline bool VectorBytesArchive::IsFlag(
			const uint8_t flag
		) const noexcept
		{
			// This byte must be an ASCII digit
			return 0 < (operator[](3) & flag);
		}

		inline bool VectorBytesArchive::IsCompressed(
			const fs::path& key
		) noexcept
		{
			constexpr array notCompressed{
				L".monad-stm",
				L".monad-lob",
				L".monad-vsi",
				L".monad-psh" };
			return ranges::find(
				notCompressed,
				key.extension()) == notCompressed.cend();
		}

		bool VectorBytesArchive::IsCompressed() const noexcept
		{
			return IsFlag(FLAG_COMPRESSED);
		}

		void VectorBytesArchive::ValidateBufferIn()
		{
			wchar_t const* excName = nullptr;
			if (size() <= sizeof FileMeta)
				excName = L"FS FileTruncated";
			else if (!between(
				MND_MAKE_FOURCC(FLAG_NONE),
				MND_MAKE_FOURCC(FLAG_COMPRESSED),
				GetHeader().m_type))
				excName = L"FS FOURCC";
			else
				return;
			THROW_EXC(
				Exceptions::FileDamaged,
				E_FAIL,
				excName);
		}

		void VectorBytesArchive::ValidateBufferOut(
			Safety::SafetyState& cipher,
			const fs::path& key
		)
		{
			wstring excName;
			if (!Hash(cipher, key))
				excName = L"Hash Validation";
			else if (GetHeader().m_sizeOfSource > ZIP_BOMB_RANGE_SIZE)
				excName = L"ZIP Bomb";
			else if (IsCompressed() ? 0 == GetHeader().m_sizeOfArchive : 0 != GetHeader().m_sizeOfArchive)
				excName = L"Compression Error";
			else if (!IsCompressed() && GetHeader().m_sizeOfSource != size() - sizeof FileMeta)
				excName = L"Incorrect Meta Data";
			else
				return;
			THROW_EXC(Exceptions::FileDamaged, E_FAIL, excName + L": " + key.wstring());
		}

		inline void VectorBytesArchive::CalcMyHash(
			Safety::SafetyState& cipher,
			const filesystem::path& key
		)
		{
			GetHeader().m_checksum = CalcHash(
				cipher,
				key
			);
		}

		void VectorBytesArchive::Decompress(
			VectorBytes& outputBuffer
		) const
		{
			outputBuffer.resize(GetHeader().m_sizeOfSource);
			DecompressLZMA(outputBuffer);
		}

		void VectorBytesArchive::DecompressAndClear(
			VectorBytes& outputBuffer
		)
		{
			Decompress(outputBuffer);
			ClearAndShrink();
		}

		inline void VectorBytesArchive::DecompressLZMA(
			VectorBytes& outputBuffer
		) const
		{
			constexpr uint8_t SZ_UNKNOWN_DICT_SIZE(40);
			SizeT
				sizeOfArchive = GetHeader().m_sizeOfArchive,
				sizeOfSource = outputBuffer.size();
			ELzmaStatus status;
			THROW_EXC_IFFAILED(
				Exceptions::FileDamaged,
				SResToHRESULT(Lzma2Decode(
					outputBuffer.data(),
					&sizeOfSource,
					Data(),
					&sizeOfArchive,
					SZ_UNKNOWN_DICT_SIZE,
					LZMA_FINISH_ANY,
					&status,
					&g_Alloc)),
				L"Decompress")
		}

		const uint8_t* VectorBytesArchive::Data() const noexcept
		{
			return data() + sizeof FileMeta;
		}

		uint8_t* VectorBytesArchive::Data() noexcept
		{
			return data() + sizeof FileMeta;
		}

		void VectorBytesArchive::Initialize(
			const uint8_t* buffer,
			size_t bufferSize,
			Safety::SafetyState& cipher,
			const filesystem::path& key
		)
		{
			// Any change of this code may malform a file manipulation in program. (some elements may be processed implicitely)
			if (IsCompressed(key))
			{
				SizeT destLength = max<size_t>(bufferSize + bufferSize / 1024 + 32, 1024);

				// set up compression properties
				CLzma2EncProps props;
				Lzma2EncProps_Init(&props);
				props.lzmaProps.level = 9;
				props.lzmaProps.mc = (1 << 30);
				props.lzmaProps.fb = 255;
				props.lzmaProps.dictSize = (1 << 26);
				props.lzmaProps.reduceSize = destLength;

				// configure multithreading based on current CPU count
				props.numBlockThreads_Max = props.numBlockThreads_Reduced
					= props.numTotalThreads
					= props.lzmaProps.numThreads
					= thread::hardware_concurrency();

				Resize(destLength);

				// create LZMA2 encoder
				struct LZMAHandle final
				{
					LZMAHandle() :
						m_lzma2Handle{
						Lzma2Enc_Create(
							&g_AlignedAlloc,
							&g_BigAlloc)
					}
						THROW_EXC_IFNULL(
							Exceptions::NotInitialized,
							m_lzma2Handle,
							L"Lzma2 Enc Create"
						)
						~LZMAHandle()
					{
						Lzma2Enc_Destroy(m_lzma2Handle);
					}
					OPER_DEL_NO_DEF_CTOR(LZMAHandle);
					inline operator const CLzma2EncHandle& () const noexcept
					{
						return m_lzma2Handle;
					}

				private:
					const CLzma2EncHandle m_lzma2Handle;
				} const HNDL;

				//set properties
				Lzma2Enc_SetProps(
					HNDL,
					&props);

				THROW_EXC_IFFAILED(
					Exceptions::InvalidClass,
					SResToHRESULT(Lzma2Enc_Encode2(
						HNDL,
						nullptr,
						Data(),
						&destLength,
						nullptr,
						buffer,
						bufferSize,
						nullptr)), L"Lzma2 Enc Encode2");
				ResizeWithPadding(destLength);
				GetHeader() =
				{
					.m_type = MND_MAKE_FOURCC(FLAG_COMPRESSED),
					.m_sizeOfArchive = static_cast<uint32_t>(size() - sizeof FileMeta),
					.m_sizeOfSource = static_cast<uint32_t>(bufferSize)
				};
			}
			else
			{
				Resize(bufferSize);
				memcpy(
					Data(),
					buffer,
					bufferSize
				);

				GetHeader() =
				{
					.m_type = MND_MAKE_FOURCC(FLAG_NONE),
					.m_sizeOfArchive = 0,
					.m_sizeOfSource = static_cast<uint32_t>(bufferSize)
				};
			}
			CalcMyHash(
				cipher,
				key
			);
		}

		static_assert(is_copy_constructible_v<VectorBytesArchive>, "Move Ctor.");
		static_assert(is_copy_assignable_v<VectorBytesArchive>, "Move Assign.");
		static_assert(is_nothrow_move_constructible_v<VectorBytesArchive>, "Move Ctor.");
		static_assert(is_nothrow_move_assignable_v<VectorBytesArchive>, "Move Assign.");
	}
}