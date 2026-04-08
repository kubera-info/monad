// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "MediaStreamer.h"
// Platform
#include <mfapi.h>
#pragma comment(lib, "Mfuuid.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
// Me
#include "Exceptions/Exceptions.h"

using Microsoft::WRL::ComPtr;
using namespace std;

namespace Monad
{
	using namespace Exceptions;
	using namespace Kernel;

	namespace Audio
	{
		namespace
		{
			inline VectorBytes GetNextBuffer(IMFSourceReader* reader)
			{
				VectorBytes resultData;

				ComPtr<IMFSample> sample;
				ComPtr<IMFMediaBuffer> mediaBuffer;

				uint8_t* audioData = nullptr;
				uint32_t sampleBufferLength = 0, flags = 0;

				THROW_EXC_AUDIO(
					reader->ReadSample(
						static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM),
						0, nullptr,
						reinterpret_cast<DWORD*>(&flags),
						nullptr,
						&sample
					),
					L"Reader Read Sample"
				);

				// Indicates the end of the audio stream.
				if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
					return resultData;
				if (!sample)
					THROW_EXC(AudioFailed, E_FAIL, L"Sample Null Value");

				THROW_EXC_AUDIO(
					sample->ConvertToContiguousBuffer(&mediaBuffer), L"Sample Convert Audio Buffer"
				);

				THROW_EXC_AUDIO(
					mediaBuffer->Lock(
						&audioData,
						nullptr,
						reinterpret_cast<DWORD*>(
							&sampleBufferLength)
					),
					L"Media Buffer Lock Audio Buffer"
				);

				//
				// Generates and returns the final vector of data.
				//
				resultData = {
					audioData,
					audioData + sampleBufferLength };

				// Release the lock
				THROW_EXC_AUDIO(
					mediaBuffer->Unlock(), L"Unlock Media Buffer"
				);

				return resultData;
			}

			inline void Restart(IMFSourceReader* reader)
			{
				PROPVARIANT var;
				ZeroMemory(&var, sizeof(var));
				var.vt = VT_I8;

				THROW_EXC_AUDIO(
					reader->SetCurrentPosition(GUID_NULL, var),
					L"Restart Stream"
				)
			}

		}
#pragma region MFManager
		MFManager::MFManager()
		{
			THROW_EXC_AUDIO(
				MFStartup(MF_VERSION, MFSTARTUP_LITE),
				L"Startup"
			)
		}

		MFManager::~MFManager()
		{
			MFShutdown();
		}
#pragma endregion MFManager

		const WAVEFORMATEX& MediaStreamer::GetOutputWaveFormatEx() const noexcept
		{
			return m_waveFornatEx;
		}

		MediaStreamer::MediaStreamer(
			IStream* const audioFileHolder
		)
		{
			ComPtr<IMFMediaType> mediaType;
			//
			// Build an attribute object to set MF_LOW_LATENCY.
			// Each title should evaluate whether the latency v. power trade-off is appropriate.
			// Longer running music tracks that are not latency sensitive may need this flag less than shorter sounds.
			// This sample is using the flag on a music track for demonstrative purposes.
			//
			ComPtr<IMFAttributes> lowLatencyAttribute;
			THROW_EXC_AUDIO(
				MFCreateAttributes(&lowLatencyAttribute, 1),
				L"Create Attributes")

				THROW_EXC_AUDIO(
					lowLatencyAttribute->SetUINT32(MF_LOW_LATENCY, TRUE),
					L"Low Latency Attribute"
				)

				//
				// Creates a source reader for the file with low-latency attributes.
				//
				ComPtr<IMFByteStream> IPByteStream;
			THROW_EXC_AUDIO(MFCreateMFByteStreamOnStream(
				audioFileHolder,
				&IPByteStream
			), L"Create Byte Stream On Stream")

				THROW_EXC_AUDIO(
					MFCreateSourceReaderFromByteStream(IPByteStream.Get(), lowLatencyAttribute.Get(), &m_reader),
					L"Create Source Reader From Byte Stream"
				)
				// Set the decoded output format as PCM
				// XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
				// When using MF, this sample always decodes into PCM.

				// Set the decoded output format as PCM
				// XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
				// When using MF, this sample always decodes into PCM.

				THROW_EXC_AUDIO(
					MFCreateMediaType(&mediaType),
					L"Create Media Type"
				)

				THROW_EXC_AUDIO(
					mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio),
					L"Set Major Type Audio GUID"
				)

				THROW_EXC_AUDIO(
					mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM),
					L"Set PCM"
				)

				THROW_EXC_AUDIO(
					m_reader->SetCurrentMediaType(static_cast<unsigned long>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, mediaType.Get()),
					L"Set Current Media Type"
				)

				// Get the complete WAVEFORMAT from the Media PODType
				ComPtr<IMFMediaType> outputMediaType;
			THROW_EXC_AUDIO(
				m_reader->GetCurrentMediaType(static_cast<unsigned long>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &outputMediaType),
				L"Get Current Media Type"
			);

			uint32_t formatByteCount = 0;
			WAVEFORMATEX* waveFormat = nullptr;
			THROW_EXC_AUDIO(
				MFCreateWaveFormatExFromMFMediaType(outputMediaType.Get(), &waveFormat, &formatByteCount),
				L"Create Wave FormatEx From Media Type"
			)
				m_waveFornatEx = *waveFormat;
			CoTaskMemFree(waveFormat);

			// Get the total length of the stream in bytes.
			PROPVARIANT var;
			THROW_EXC_AUDIO(
				m_reader->GetPresentationAttribute(static_cast<unsigned long>(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &var),
				L"Get Presentation Attribute"
			)
				m_maxStreamLengthInBytes = static_cast<uint32_t>(var.uhVal.QuadPart / (10000.0 * 1000.0) * m_waveFornatEx.nAvgBytesPerSec);
		}

		VectorBytes MediaStreamer::ReadAll()
		{
			Restart(m_reader.Get());
			VectorBytes resultData, nextBuffer;
			resultData.reserve(m_maxStreamLengthInBytes);

			for (;;)
			{
				nextBuffer = GetNextBuffer(m_reader.Get());
				if (nextBuffer.empty())
					break;

				// Append the new buffer to the running total.
				resultData.append_range(nextBuffer);
			}

			return resultData;
		}
	}
}
