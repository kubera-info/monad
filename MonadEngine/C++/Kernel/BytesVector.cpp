// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "BytesVector.h"
// Platform
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

namespace Monad::Kernel
{
#pragma region SpanConstBytes
	PtrIStream SpanConstBytes::GetComIStreamTemp() const
	{
		/// <summary>
		/// Class sugested by claude.ai
		/// </summary>
		struct MemoryStream : IStream
		{
		private:
			LONG m_refCount;
			SpanConstBytes m_buffer;  // Reference to external buffer
			ULONGLONG m_position;

		public:
			// Constructor takes a reference to an existing buffer
			explicit MemoryStream(
				SpanConstBytes buffer
			) noexcept :
				m_refCount(1),
				m_buffer(buffer),
				m_position(0)
			{}

			// IUnknown methods
			STDMETHODIMP QueryInterface(
				REFIID riid,
				void** ppvObject
			) noexcept final
			{
				if (!ppvObject) return E_INVALIDARG;

				*ppvObject = nullptr;
				if (riid == IID_IUnknown || riid == IID_ISequentialStream || riid == IID_IStream) {
					*ppvObject = static_cast<IStream*>(this);
					AddRef();
					return S_OK;
				}
				return E_NOINTERFACE;
			}

			STDMETHODIMP_(ULONG) AddRef() noexcept final
			{
				return InterlockedIncrement(&m_refCount);
			}

			STDMETHODIMP_(ULONG) Release() noexcept final
			{
				LONG count = InterlockedDecrement(&m_refCount);
				if (count == 0) {
					delete this;
					return 0;
				}
				return count;
			}

			// ISequentialStream methods
			STDMETHODIMP Read(
				void* pv,
				ULONG cb,
				ULONG* pcbRead
			) noexcept final
			{
				if (!pv) return STG_E_INVALIDPOINTER;

				ULONG bytesRead = 0;
				if (m_position < m_buffer.size())
				{
					bytesRead = std::min<ULONG>(cb, static_cast<ULONG>(m_buffer.size() - m_position));
					memcpy(pv, &m_buffer[static_cast<size_t>(m_position)], bytesRead);
					m_position += bytesRead;
				}

				if (pcbRead) *pcbRead = bytesRead;
				return (bytesRead == cb) ? S_OK : S_FALSE;
			}

			STDMETHODIMP Write(
				[[maybe_unused]] const void* pv,
				[[maybe_unused]] ULONG cb,
				[[maybe_unused]] ULONG* pcbWritten
			) noexcept final
			{
				/*if (!pv && cb > 0) return STG_E_INVALIDPOINTER;

				ULONGLONG newSize = m_position + cb;
				if (newSize > m_buffer.size()) {
					m_buffer.resize(static_cast<size_t>(newSize));
				}

				memcpy(&m_buffer[static_cast<size_t>(m_position)], pv, cb);
				m_position += cb;

				if (pcbWritten) *pcbWritten = cb;
				return S_OK;*/
				return E_NOTIMPL;  // Not supported for read-only memory stream
			}

			// IStream methods
			STDMETHODIMP Seek(
				LARGE_INTEGER dlibMove,
				DWORD dwOrigin,
				ULARGE_INTEGER* plibNewPosition
			) noexcept final
			{
				LONGLONG newPos = 0;

				switch (dwOrigin) {
				case STREAM_SEEK_SET:
					newPos = dlibMove.QuadPart;
					break;
				case STREAM_SEEK_CUR:
					newPos = m_position + dlibMove.QuadPart;
					break;
				case STREAM_SEEK_END:
					newPos = m_buffer.size() + dlibMove.QuadPart;
					break;
				default:
					return STG_E_INVALIDFUNCTION;
				}

				if (newPos < 0) return STG_E_INVALIDFUNCTION;

				m_position = static_cast<ULONGLONG>(newPos);
				if (plibNewPosition)
					plibNewPosition->QuadPart = m_position;
				return S_OK;
			}

			STDMETHODIMP SetSize(
				[[maybe_unused]] ULARGE_INTEGER libNewSize
			) noexcept final
			{
				/*m_buffer.resize(static_cast<size_t>(libNewSize.QuadPart));
				return S_OK;*/
				return E_NOTIMPL;  // Not supported for read-only memory stream
			}

			STDMETHODIMP CopyTo(
				IStream* pstm,
				ULARGE_INTEGER cb,
				ULARGE_INTEGER* pcbRead,
				ULARGE_INTEGER* pcbWritten
			) noexcept final
			{
				if (!pstm) return STG_E_INVALIDPOINTER;

				ULONGLONG bytesToCopy = std::min<size_t>(cb.QuadPart, m_buffer.size() - m_position);
				ULONG written = 0;

				HRESULT hr = pstm->Write(&m_buffer[static_cast<size_t>(m_position)],
					static_cast<ULONG>(bytesToCopy), &written);

				if (SUCCEEDED(hr))
				{
					m_position += written;
					if (pcbRead) pcbRead->QuadPart = written;
					if (pcbWritten) pcbWritten->QuadPart = written;
				}
				return hr;
			}

			STDMETHODIMP Commit(
				DWORD
			) noexcept final
			{
				return S_OK;  // No-op for memory stream
			}

			STDMETHODIMP Revert() noexcept override {
				return E_NOTIMPL;  // Not supported for simple memory stream
			}

			STDMETHODIMP LockRegion(
				ULARGE_INTEGER,
				ULARGE_INTEGER,
				DWORD
			) noexcept final
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP UnlockRegion(
				ULARGE_INTEGER,
				ULARGE_INTEGER,
				DWORD
			) noexcept final
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP Stat(
				STATSTG* pstatstg,
				[[maybe_unused]] DWORD grfStatFlag
			) noexcept final
			{
				if (!pstatstg) return STG_E_INVALIDPOINTER;

				ZeroMemory(pstatstg, sizeof(STATSTG));
				pstatstg->type = STGTY_STREAM;
				pstatstg->cbSize.QuadPart = m_buffer.size();
				return S_OK;
			}

			STDMETHODIMP Clone(
				IStream**
			) noexcept final
			{
				return E_NOTIMPL;
			}
		};
		PtrIStream fileStream;
		fileStream.Attach(new MemoryStream(*this));
		return fileStream;
	}

	PtrIStream SpanConstBytes::GetComIStreamOnCopy() const
	{
		PtrIStream fileStream;
		fileStream.Attach(SHCreateMemStream(
			data(),
			static_cast<uint32_t>(size()))
		);
		if (!fileStream)
			throw std::bad_alloc();
		return fileStream;
	}
#pragma endregion

#pragma region VectorBytes
	VectorBytes::VectorBytes(
		const SpanConstBytes sourceBytes
	) :
		VectorNoCtor(
			sourceBytes.cbegin(),
			sourceBytes.cend()
		)
	{}
#pragma endregion

#pragma region VectorBlock
	VectorBlock::VectorBlock(
		size_t blockSize
	) noexcept :
		c_blockSize{ blockSize }
	{}

	void VectorBlock::resize(size_t blocks)
	{
		__super::resize(blocks * c_blockSize);
	}

	size_t VectorBlock::size() const noexcept
	{
		return __super::size() / c_blockSize;
	}

	void* VectorBlock::data() noexcept
	{
		return __super::data();
	}

	const void* VectorBlock::data() const noexcept
	{
		return __super::data();
	}
#pragma endregion
}