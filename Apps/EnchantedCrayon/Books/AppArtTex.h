// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "AppColorIndex.h"
#include "Files/Struct.h"
#include "Threads/ThreadPool.h"

namespace Monad::App
{
	/// <summary>
	/// Represents a single artwork resource.
	/// 
	/// This class encapsulates:
	///  - a compressed DDS texture,
	///  - a CPU-side decompressed buffer,
	///  - and lookup logic for mapping texels to palette indices.
	///
	/// The texture stores color indices as 16-bit values used during rendering.
	/// </summary>
	struct TaskAndMemoryLookup final : Threads::ThreadPool::ITask
	{
		/// <summary>
		/// Constructs the task using a compressed DDS archive.
		/// The data is not decompressed until Run() is executed.
		/// </summary>
		/// <param name="file">DDS file stored as an archived byte buffer</param>
		TaskAndMemoryLookup(
			Files::VectorBytesArchive&& file
		) noexcept;

		~TaskAndMemoryLookup() = default;
		OPER_DEL(TaskAndMemoryLookup);

		/// <summary>
		/// Returns the palette index for a given normalized texture coordinate.
		/// </summary>
		/// <param name="x">Horizontal coordinate in range [0..1]</param>
		/// <param name="y">Vertical coordinate in range [0..1]</param>
		/// <returns>Palette index corresponding to the texel</returns>
		size_t GetColorIndex(
			float x,
			float y
		) const noexcept;

		/// <summary>
		/// Returns the texture width in texels.
		/// </summary>
		uint32_t GetWidth() const noexcept;

		/// <summary>
		/// Returns the texture height in texels.
		/// </summary>
		uint32_t GetHeight() const noexcept;

		/// <summary>
		/// Decompresses the archived DDS data into CPU memory.
		/// Executed inside the thread pool.
		/// </summary>
		void Run() final;

		/// <summary>
		/// Returns the decompressed CPU buffer.
		/// </summary>
		const Kernel::VectorBytes& GetCPUBuffer() const noexcept;

		/// <summary>
		/// Frees the decompressed buffer while keeping the compressed archive.
		/// Useful for reducing memory usage after upload.
		/// </summary>
		void ClearAndShrink() noexcept;

		/// <summary>
		/// Color palette extracted from the texture.
		/// Used for mapping indexed pixels to final colors.
		/// </summary>
		ArrayColorsGeneric m_palette;

	private:
		/// <summary>
		/// CPU-side decompressed DDS data.
		/// </summary>
		Kernel::VectorBytes m_bufferCPU;

		/// <summary>
		/// Original compressed DDS archive.
		/// Preserved for possible reuse or reprocessing.
		/// </summary>
		const Files::VectorBytesArchive c_archiveBuffer;
	};
}