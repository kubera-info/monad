// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "FileManager.h"
// STD
#include <ranges>
// Monad
#include "Core/DXSample.h"
#include "Op.h"
#include "System/Exit.h"
#include "Tools/Convert.h"

using namespace std;
namespace fs = filesystem;

namespace Monad
{
	using namespace Exceptions;
	using namespace Kernel;
	using namespace System;
	using namespace Safety;
	using namespace Threads;
	using namespace Tools;

	namespace
	{
		Files::Path FONT_PATH{ DummyParameter{}, "Font" };
	}

	namespace Files
	{
		VectorPath g_texturesExts;
		SingleCall g_profileOnce;
		atomic_size_t g_fileManagerState = FILE_MAN_STATE_LOAD_BOOT;
		fs::path g_profilePath;
		/// <summary>
		/// Removes the profile file if an exception occurs during processing.
		/// </summary>
		void DropProfile() noexcept
		{
#if defined(MONAD_DROP_FILE)
			if (!g_profilePath.empty())
			{
				std::error_code ec;
				std::filesystem::remove(g_profilePath, ec);
			}
#endif
		}

#pragma region TaskFileHolderGeneric
		TaskFileHolderGeneric::TaskFileHolderGeneric(
			TaskFileSearcherGeneric* const fileSearcher,
			fs::path&& filePath,
			const DWORD fileSize,
			const bool ifDelete
		) noexcept :
			ITask{ ifDelete },
			//c_sequenceNo{ fileSearcher->m_sequenceNo.load() },
			m_fileSearcher{ fileSearcher },
			c_path{ move(filePath) },
			c_sizeOfBuffer{ fileSize }
		{
		}

		TaskFileHolderGeneric::TaskFileHolderGeneric(
			TaskFileSearcherGeneric* const fileSearcher,
			fs::path&& filePath
		) noexcept :
			TaskFileHolderGeneric(
				fileSearcher,
				move(filePath),
				0,
				false
			)
		{
		}
#define WITHOUT_META { m_archiveBuffer.cbegin() + sizeof VectorBytesArchive::FileMeta, m_archiveBuffer.cend() }

		const SpanConstBytes TaskFileHolderGeneric::GetSpan() const noexcept
		{
			if (HasArchive())
				return WITHOUT_META;
			else
				return m_outputBuffer;
		}

		VectorBytes TaskFileHolderGeneric::GetVector()
		{
			if (HasArchive())
				return WITHOUT_META;
			else
				return move(m_outputBuffer);
		}

		wstring TaskFileHolderGeneric::Split::UTF8ToUnicode() const
		{
			return UTF8ToUnicodeView(c_stringView);
		}

		PtrIStream TaskFileHolderGeneric::GetComIStreamTemp() const
		{
			return GetSpan().GetComIStreamTemp();
		}

		PtrIStream TaskFileHolderGeneric::GetComIStreamOnCopy() const
		{
			return GetSpan().GetComIStreamOnCopy();
		}

		void TaskFileHolderGeneric::Run()
		{
			__try
			{
				OnFile();
			}
			__except (filter(
				GetExceptionCode(),
				GetExceptionInformation())
				)
			{
				DropProfile();
				Shutdown(EXIT_FAILURE);
			}
		}

		wstring TaskFileHolderGeneric::GetWFilename() const
		{
			return c_path.stem();
		}

		string TaskFileHolderGeneric::GetFilename() const
		{
			return c_path.stem().string();
		}

		bool TaskFileHolderGeneric::HasArchive() const noexcept
		{
			return !m_archiveBuffer.empty();
		}
#pragma endregion

#pragma region TaskFileHolder
		void TaskFileHolder::OnFile()
		{
			if (!InAnyCloseReason())
				try
				{
					const auto filename = c_path.filename();
					const wstring ext = filename.extension();
					if (const auto extExecIt = m_fileSearcher->c_interpreter.find(ext); m_fileSearcher->c_interpreter.cend() != extExecIt)
					{
						{
							const SmartHandleFile fileReader(
								c_path,
								GENERIC_READ,
								FILE_SHARE_READ,
								nullptr,
								OPEN_EXISTING,
								0
							);
							m_archiveBuffer.resize(0 == c_sizeOfBuffer ? fileReader.FileSize() : c_sizeOfBuffer);
							fileReader.Read(m_archiveBuffer);
						}
						if (IsMonadExt(ext))
						{
							m_fileSearcher->m_cipher.GetClear(filename, m_archiveBuffer);
							if (L".monad-art" != ext && m_archiveBuffer.IsCompressed())
								m_archiveBuffer.DecompressAndClear(m_outputBuffer);
						}
						else
							m_outputBuffer = move(m_archiveBuffer);
						extExecIt->second(*this);
					}
				}
				catch (...)
				{
					DropProfile();
					throw;
				}
		}
#pragma endregion

#pragma region TaskFileSearcherGeneric
		TaskFileSearcherGeneric::TaskFileSearcherGeneric(
			InitializerListFns interpreter,
			SafetyState& cipher,
			ThreadPool& threadPool) :
			ITask{ false },
			c_me{ this, g_fileSearcherGeneric },
			m_called{ true },
			c_interpreter{ interpreter },
			m_cipher{ cipher },
			m_threadPool{ threadPool }
		{
		}

		TaskFileSearcherGeneric::~TaskFileSearcherGeneric()
		{
			WaitForStateCompletion();
		}

		void TaskFileSearcherGeneric::WaitForStateCompletion() noexcept
		{
			g_fileManagerState = FILE_MAN_STATE_FREE;
			while (1 < GetRefCount())
				SwitchToThread();
		}

		bool TaskFileSearcherGeneric::IsNotReferenced() noexcept
		{
			if (0 == m_stateRefsCount)
				return true;
			else if (0 == InterlockedDecrement64(&m_stateRefsCount))
				if (InAnyCloseReason())
					g_fileManagerState = FILE_MAN_STATE_EXCEPTION;
				else
					return true;
			return false;
		}

		void TaskFileSearcherGeneric::SetCalled() noexcept
		{
			m_called = true;
		}

		void TaskFileSearcherGeneric::RunSearch(
			VectorPath searchPaths
		)
		{
			m_called = false;

			ThreadPool::ListPtrsTasks listOfPtrsHolders;
			{
				struct FileHandleAssetLoader final : FileHandle
				{
					FileHandleAssetLoader(
						VectorPath searchPaths,
						TaskFileSearcherGeneric* const fileSearcher,
						ThreadPool::ListPtrsTasks& listOfPointers
					) :
						FileHandle{ searchPaths },
						m_fileSearcher{ fileSearcher },
						m_listOfPtrs{ listOfPointers }
					{
					}
					void OnFindFile(
						const filesystem::path& selectedFolder
					) final
					{
						const fs::path fileName = ToLower(FindFileData().cFileName);
						const wstring ext = fileName.extension();
						if (m_fileSearcher->c_interpreter.contains(ext)) [[likely]]
						{
							ThreadPool::PtrTask file;
							file.Attach(new FileRefHolder(
								m_fileSearcher,
								selectedFolder / fileName,
								FindFileData().nFileSizeLow)
							);
							m_listOfPtrs.emplace_front(file);
							m_fileSearcher->IncrementRefCount();
						}
					}

				private:
					TaskFileSearcherGeneric* const m_fileSearcher;
					ThreadPool::ListPtrsTasks& m_listOfPtrs;
				} handleLoader{ searchPaths, this, listOfPtrsHolders };
				handleLoader.Run();
			}
			if (!listOfPtrsHolders.empty())
			{
				m_threadPool.InsertTasks(listOfPtrsHolders | views::transform
				(
					[](const auto& holder) noexcept
					{
						return holder.Get();
					}
				));
				for (auto& holder : listOfPtrsHolders)
					holder.Detach(); // Must be after insert, not before, because of RAII.
			}
			else
				IsNotReferenced();
		}
		void TaskFileSearcherGeneric::IncrementRefCount() noexcept
		{
			InterlockedIncrement64(&m_stateRefsCount);
		}

		void TaskFileSearcherGeneric::OnFinish() noexcept
		{
			if (IsNotReferenced())
			{
				OnStateChange();
				SetCalled();
			}
		}

		fs::path TaskFileSearcherGeneric::GetFontPath(const std::string& fontFace)
		{
			return FONT_PATH / (fontFace + ".monad-fnt"s);
		}

		uint32_t TaskFileSearcherGeneric::SetShaderResourceViewsCount()
		{
			return FileHandleCount{
				{ GetFolderRes() }
			};
		}

		TaskFileSearcherGeneric* g_fileSearcherGeneric = nullptr;
#pragma endregion

#pragma region FileRefHolder
		FileRefHolder::FileRefHolder(
			TaskFileSearcherGeneric* fileSearcher,
			fs::path&& filePath,
			const DWORD fileSize
		) noexcept :
			TaskFileHolder{
			fileSearcher,
			move(filePath),
			fileSize,
			true
		}
		{
		}

		FileRefHolder::~FileRefHolder()
		{
			m_fileSearcher->OnFinish(/*c_sequenceNo*/);
		}
#pragma endregion

#pragma region FileHandleCount
		FileHandleCount::FileHandleCount(
			const Kernel::VectorPath& searchFolders
		) :
			FileHandle(ranges::to<Kernel::VectorPath>(
				std::views::transform(searchFolders,
					[](const auto& newPath) { return newPath / WILDCARD_ANY; }))
			),
			m_count(0)
		{
			Run();
		}

		FileHandleCount::FileHandleCount(
			Kernel::DummyParameter,
			const Kernel::VectorPath& searchFolders
		) :
			FileHandle(
				searchFolders
			),
			m_count(0)
		{
			Run();
		}

		void FileHandleCount::OnFindFile(const fs::path&)
		{
			if (ranges::find(
				g_texturesExts,
				fs::path(ToLower(FindFileData().cFileName)).extension())
				!= g_texturesExts.cend())
				++m_count;
		}

		void FileHandleCount::OnFindDirectory(const fs::path& selectedFolder)
		{
			m_count += FileHandleCount{
				Kernel::DummyParameter{},
				{ selectedFolder / FindFileData().cFileName / WILDCARD_ANY }
			};
		}

		FileHandleCount::operator uint32_t() const
		{
			THROW_EXC_IFFALSE(Exceptions::OutOfRange, m_count < 10001, L"Cannot Count Files");
			return m_count;
		}
#pragma endregion
	}
}
