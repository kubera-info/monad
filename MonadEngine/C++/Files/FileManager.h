// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Dummy.h"
#include "Kernel/Templates.h"
#include "Struct.h"
#include "Handle.h"
#include "Folder.h"
#include "Threads/ThreadPool.h"
#include "Tools/SingleCall.h"
#include "Repository/Repo.h"
#include "Fonts/FontLoader.h"
#include "Audio/Persistent.h"

namespace Monad::Files
{
	/// <summary>
	/// File extensions used by the engine.
	/// These represent encrypted or packed texture formats.
	/// </summary>
	inline constexpr const wchar_t
		* EXT_ART = L".monad-art",
		* EXT_FONT = L".monad-fnt",
		* EXT_TEX = L".monad-tex",
		* EXT_DDS = L".dds",
		* EXT_JPG = L".jpg",
		* EXT_PNG = L".png";

	extern Kernel::VectorPath g_texturesExts;

	struct FileRefHolder;
	struct TaskFileHolder;
	struct TaskFileHolderGeneric;

	/// <summary>
	/// Profile data is loaded a single time by the File Manager for each execution of the application.
	/// </summary>
	extern Tools::SingleCall g_profileOnce;
	extern const std::filesystem::path WILDCARD;
	extern std::filesystem::path g_profilePath;
	constexpr size_t
		FILE_MAN_STATE_FREE = 2048,
		FILE_MAN_STATE_LOAD_BOOT = 4096,
		FILE_MAN_STATE_EXCEPTION = 8192,
		FILE_MAN_STATE_READY = 16348;
	/// <summary>
	/// Current state of the File Loader/Searcher.
	/// </summary>
	extern std::atomic_size_t g_fileManagerState;

	/// <summary>
	/// Queries filenames and file sizes of assets.
	/// </summary>
	interface TaskFileSearcherGeneric abstract : Threads::ThreadPool::ITask
	{
		friend FileRefHolder;
		friend TaskFileHolder;
		friend TaskFileHolderGeneric;

		~TaskFileSearcherGeneric() override;
		OPER_DEL(TaskFileSearcherGeneric);
		/// <summary>
		/// Paths with wildcards to load.
		/// </summary>
		void RunSearch(
			Kernel::VectorPath searchPaths
		);
		/// <summary>
		/// Adds a reference to a file for processing (not yet completed).
		/// </summary>
		void WaitForStateCompletion() noexcept;

	protected:
		static std::filesystem::path GetFontPath(const std::string& fontFace);
		void IncrementRefCount() noexcept;
		virtual void OnStateChange() noexcept = 0;
		virtual uint32_t SetShaderResourceViewsCount() = 0;

		/// <summary>
		/// Asset-loading threads operate without blocking synchronization.
		/// </summary>
		void OnFinish() noexcept;
		/// <summary>
		/// Defines the signature required to implement a file processing function.
		/// </summary>
		using FnProcessFileHolder = void(*)(TaskFileHolderGeneric& file);
		/// <summary>
		/// Maps a file extension to its corresponding processing function.
		/// </summary>
		using MapInterpreter = Kernel::UnorderedMapWString<const FnProcessFileHolder>;
		/// <summary>
		/// Collection of mappings from file extensions to their implemented loading functions.
		/// </summary>
		using InitializerListFns = std::initializer_list<MapInterpreter::value_type>;

		/// <summary>
		// Defines state machine handlers for processing different file extensions.
		/// </summary>
		TaskFileSearcherGeneric(
			InitializerListFns interpreter,
			Safety::SafetyState& cipher,
			Threads::ThreadPool& threadPool
		);

		/// <summary>
		/// Caution: invoking this function mutates the object state and updates a global variable.
		/// </summary>
		/// <returns>Returns true if there are no remaining references and the state machine should transition to another state.</returns>
		bool IsNotReferenced() noexcept;
		void SetCalled() noexcept;

	private:
		const Kernel::Me<TaskFileSearcherGeneric> c_me;
		volatile long long m_stateRefsCount = 0;
		std::atomic_bool m_called;
		const MapInterpreter c_interpreter;
		Threads::ThreadPool& m_threadPool;
		Safety::SafetyState& m_cipher;
	} extern* g_fileSearcherGeneric;

	/// <summary>
	/// Encapsulates loading and storage of a binary file in memory.
	/// A generic file holder for clear or encoded asset files.
	/// </summary>
	interface __declspec(novtable) TaskFileHolderGeneric abstract : Threads::ThreadPool::ITask
	{
		TaskFileHolderGeneric(
			TaskFileSearcherGeneric* const fileSearcher,
			std::filesystem::path && filePath
		) noexcept;
		OPER_DEL(TaskFileHolderGeneric);

		std::wstring GetWFilename() const; // The filename without the extension
		std::string GetFilename() const; // The filename without the extension
		bool HasArchive() const noexcept;
		const Kernel::SpanConstBytes GetSpan() const noexcept;
		Kernel::VectorBytes GetVector();
		/// <summary>
		/// Gets LPSTRZ + blob
		/// Returns std::string_view + blob's span
		/// Warning: temporary storage
		/// </summary>
		struct Split
		{
			template<typename V = Kernel::SpanConstBytes>
			Split(
				const V& sourceSpan
			) noexcept :
				c_stringView{ reinterpret_cast<const char*>(sourceSpan.data()) },
				c_span{
					sourceSpan.subspan(c_stringView.length() + 1,
						sourceSpan.size() - c_stringView.length() - 1)
				}
			{}
			std::wstring UTF8ToUnicode() const;

			const std::string_view c_stringView;
			const Kernel::SpanConstBytes c_span;
		};
		/// <summary>
		/// Splits the file’s data vector into two parts: a string from the beginning and a binary blob from the end.
		/// </summary>
		/// <returns>Pairs a string view with a following binary blob span.</returns>
		inline Split GetSplit() const noexcept
		{
			return GetSpan();
		}

		/// <summary>
		/// Returns intialized IStream Com
		/// </summary>
		Kernel::PtrIStream GetComIStreamTemp() const;
		Kernel::PtrIStream GetComIStreamOnCopy() const;
		/// <summary>
		/// The single file processor
		/// </summary>
		virtual void OnFile() = 0;
		void Run() final;

		/// <summary>
		/// Binary data block stored in a compressed form (LZMA).
		/// </summary>
		VectorBytesArchive m_archiveBuffer;

	protected:
		TaskFileHolderGeneric(
			TaskFileSearcherGeneric* const fileSearcher,
			std::filesystem::path&& filePath,
			const DWORD fileSize,
			const bool ifDelete
		) noexcept;

		/// <summary>
		/// Binary clear data block=.
		/// </summary>
		Kernel::VectorBytes m_outputBuffer;
		Microsoft::WRL::ComPtr<TaskFileSearcherGeneric> m_fileSearcher;

	public:
		const std::filesystem::path c_path;

	protected:
		const uint32_t c_sizeOfBuffer;
	};

	/// <summary>
	/// An encrypted version of the file holder.
	/// </summary>
	struct TaskFileHolder : TaskFileHolderGeneric
	{
		using TaskFileHolderGeneric::TaskFileHolderGeneric;

	private:
		/// <summary>
		/// The single file processor.
		/// </summary>
		void OnFile() final; // Loads declared file into the virtual memory
	};

	struct FileRefHolder final : TaskFileHolder
	{
		FileRefHolder(
			TaskFileSearcherGeneric* fileSearcher,
			std::filesystem::path&& filePath,
			const DWORD fileSize
		) noexcept;
		~FileRefHolder() override;
		OPER_DEL(FileRefHolder);
	};
	void DropProfile() noexcept;

	struct FileHandleCount final : FileHandle
	{
		FileHandleCount(
			const Kernel::VectorPath& searchFolders
		);
		void OnFindFile(
			const std::filesystem::path& selectedFolder
		) final;
		void OnFindDirectory(
			const std::filesystem::path& selectedFolder
		) final;
		[[nodiscard]]
		operator uint32_t() const;

	private:
		FileHandleCount(
			Kernel::DummyParameter,
			const Kernel::VectorPath& searchFolders
		);

		uint32_t m_count;
	};

	const auto LOADER_MESH = [](TaskFileHolderGeneric& file)
		{
			Monad::Repositories::g_repositoryGeneric->m_meshes.Insert(
				file.GetFilename(),
				file.GetSpan());
		};

	const auto LOADER_PCM = [](TaskFileHolderGeneric& file)
		{
			Audio::g_persistentAudio->CreateSound(file.GetFilename(), file.GetVector());
		};

	const auto LOADER_MP3 = [](TaskFileHolderGeneric& file)
		{
			Audio::g_persistentAudio->CreateStream(file.GetFilename(), file.GetComIStreamOnCopy().Get()); \
		};

	const auto LOADER_TEX = [](TaskFileHolderGeneric& file)
		{
			Repositories::g_repositoryGeneric->m_shaderResources.Insert(
				file.GetFilename(),
				file.GetSpan());
		};

	const auto LOADER_FONT = [](TaskFileHolderGeneric& file)
		{
			const auto canonicalSplit = file.GetSplit();
			Fonts::LoadXMLFont(
				Kernel::SpanConstBytes(canonicalSplit.c_stringView).GetComIStreamTemp().Get(),
				file.GetFilename());
			Repositories::g_repositoryGeneric->m_shaderResources.Insert(
				file.GetFilename(),
				canonicalSplit.c_span);
		};

	const auto LOADER_VS_IL = [](TaskFileHolderGeneric& file)
		{
			const auto canonicalSplit = file.GetSplit();
			Repositories::g_repositoryGeneric->m_vs.Insert(
				file.GetFilename(),
				canonicalSplit.c_span,
				std::string(canonicalSplit.c_stringView));
		};

	const auto LOADER_PS = [](TaskFileHolderGeneric& file)
		{
			Repositories::g_repositoryGeneric->m_ps.Insert(file.GetFilename(), file.GetVector());
		};

	const auto LOADER_DUMMY = []([[maybe_unused]] TaskFileHolderGeneric& file)
		{
		};
}
