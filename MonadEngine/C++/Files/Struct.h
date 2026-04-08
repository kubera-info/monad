// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <Windows.h>
#include <mmsystem.h>
// Monad
#include "Safety/SafetyManager.h"

namespace Monad::Files
{
	/// <summary>
	/// A vector capable of compressing and decompressing data.
	/// </summary>
	struct VectorBytesArchive final : Kernel::VectorBytes
	{
		using VectorBytes::VectorBytes;
		~VectorBytesArchive() = default;
		OPER_DEF_NO_DEF_CTOR(VectorBytesArchive);

		/// <summary>
		/// Header of an asset file (not encrypted).
		/// </summary>
		/// <param name="X">Meta-bits configuration describing storage properties.</param>
		/// <returns>Final FOURCC value</returns>
		static consteval DWORD MND_MAKE_FOURCC(const uint8_t X) noexcept { return MAKEFOURCC('M', 'N', 'D', '0' | X); }

		struct FileMeta
		{
			FOURCC m_type = 0;
			Safety::ArrayKey256Bits m_checksum{};
			uint32_t m_sizeOfArchive = 0, m_sizeOfSource = 0;
		};

		inline FileMeta& GetHeader() noexcept
		{
			return *reinterpret_cast<FileMeta*>(data());
		}
		inline const FileMeta& GetHeader() const noexcept
		{
			return *reinterpret_cast<const FileMeta*>(data());
		}
		bool Hash(
			Safety::SafetyState& cipher, const std::filesystem::path& key
		);
		void Decompress(
			Kernel::VectorBytes& outputBuffer
		) const;
		void DecompressAndClear(
			Kernel::VectorBytes& outputBuffer
		);
		template<typename V>
		inline VectorBytesArchive(
			const V& inputBuffer,
			Safety::SafetyState& cipher,
			const std::filesystem::path& key)
		{
			Initialize(
				reinterpret_cast<const uint8_t*>(inputBuffer.data()),
				inputBuffer.size() * sizeof V::value_type,
				cipher,
				key
			);
		}
		inline bool IsFlag(
			const uint8_t flag
		) const noexcept;
		static inline bool IsCompressed(
			const std::filesystem::path& key
		) noexcept;
		bool IsCompressed() const noexcept;
		constexpr static inline FOURCC Flag(
			uint8_t ch3
		) noexcept { return (static_cast<FOURCC>(static_cast<uint8_t>(ch3) << 24)); }
		void ValidateBufferIn();
		void ValidateBufferOut(
			Safety::SafetyState& cipher,
			const std::filesystem::path& key
		);

	private:
		void Initialize(
			const uint8_t* buffer,
			size_t bufferSize,
			Safety::SafetyState& cipher,
			const std::filesystem::path& key
		);
		inline void ResizeWithPadding(
			size_t newSize
		);
		inline void Resize(
			size_t newSize
		);
		inline void CalcMyHash(
			Safety::SafetyState& cipher,
			const std::filesystem::path& key
		);
		inline void DecompressLZMA(
			Kernel::VectorBytes& outputBuffer
		) const;
		const uint8_t* Data() const noexcept;
		uint8_t* Data() noexcept;

		Safety::ArrayKey256Bits CalcHash(
			Safety::SafetyState& cipher,
			const std::filesystem::path& key
		);
	};
}
