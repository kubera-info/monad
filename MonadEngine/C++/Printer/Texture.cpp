// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Texture.h"

// Platform
#include <d3d11on12.h>
#include <Prntvpt.h>
#pragma comment(lib,"D3D11.lib")

// Monad
#include "Globals/__MonadGlobals.h"
#include "Audio/Persistent.h"
#include "Core/DXSample.h"
#include "Core/Win32ApplicationGeneric.h"
#include "Files/Temporary.h"
#include "Kernel/BytesVector.h"
#include "Kernel/Format.h"
#include "D2DPrintJobChecker.h"
#include "Pages/Page.h"
#include "Printer/Sketch.h"
#include "Renderer2D/2D.h"

using Microsoft::WRL::ComPtr;

namespace Monad
{
	using namespace Exceptions;
	using namespace GUI;
	using namespace I18N;
	using namespace Kernel;

	namespace Print
	{
		namespace
		{
			auto LambdaDestroyDevMode = [](DEVMODE* const devMode) noexcept
				{
					assert(nullptr != devMode);
					delete[] reinterpret_cast<uint8_t* const>(devMode);
				};
			struct TexturePrinter
			{
				inline TexturePrinter(DirectX::ScratchImage&& outputImage) noexcept;
				inline ~TexturePrinter();
				inline void Run() noexcept;
				inline HRESULT CreateDeviceContext() noexcept;
				inline HRESULT InitializePrintJob() noexcept;
				inline HRESULT GetPrintTicketFromDevmode(
					PCTSTR printerName,
					PDEVMODE devMode,
					WORD devModeSize,
					LPSTREAM* printTicketStream
				) noexcept;
				inline HRESULT OnPrint() noexcept;
				/// <summary>
				/// Draws the scene to a rendering device context or a printing device context.
				/// If the "printing" parameter is set, this function will add margins to
				/// the target and render the text across two pages. Otherwise, it fits the
				/// content to the target and renders the text in one block.
				/// </summary>
				/// <param name="d2dContext">Direct2D Context</param>
				/// <returns></returns>
				inline HRESULT DrawToContext(
					ID2D1DeviceContext* d2dContext
				) noexcept;
				inline HRESULT FinalizePrintJob() noexcept;
				inline void DiscardDeviceResources() noexcept;

				DirectX::ScratchImage m_outputImage;

				float m_pageHeight = 1920.0f;
				float m_pageWidth = 1080.0f;
				ComPtr<ID3D11On12Device> m_d3d11On12Device;
				ComPtr<ID2D1Device> m_d2dDevice;
				using FnDestroyDevMode = void(*)(DEVMODE* const) noexcept;
				std::unique_ptr<DEVMODE, FnDestroyDevMode> m_devMode;
				std::unique_ptr<D2DPrintJobChecker> m_printJobChecker;
				ComPtr<IPrintDocumentPackageTarget> m_documentTarget;
				Kernel::PtrIStream m_jobPrintTicketStream;
				ComPtr<ID2D1PrintControl> m_printControl;
				ComPtr<ID2D1Bitmap> m_customBitmap;
			};

			TexturePrinter::TexturePrinter(DirectX::ScratchImage&& outputImage) noexcept :
				m_outputImage{ std::move(outputImage) },
				m_devMode{ nullptr, LambdaDestroyDevMode }
			{}

			TexturePrinter::~TexturePrinter()
			{
				Win32ApplicationGeneric::ActivateDirect3DWindow();
				Pages::SelectPage("game"_pageNo);
			}

			HRESULT TexturePrinter::CreateDeviceContext() noexcept
			{
				// Create a D3D device and a swap chain sized to the child window.
#ifdef _DEBUG
				constexpr uint32_t createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#else
				constexpr uint32_t createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
				ComPtr<ID3D11Device> d3d11Device;
				V_RETURN(D3D11On12CreateDevice(
					g_dxSample->m_device.Get(),
					createDeviceFlags,
					nullptr,
					0,
					reinterpret_cast<IUnknown**>(g_dxSample->m_commandQueue.GetAddressOf()),
					1,
					0,
					&d3d11Device,
					nullptr,
					nullptr
				));
				V_RETURN(d3d11Device.As(&m_d3d11On12Device));
				ComPtr<IDXGIDevice> dxgiDevice;
				V_RETURN(m_d3d11On12Device.As(&dxgiDevice));
				// Create a D2D device from the DXGI device.
				return Renderer2D::g_factory2D->m_d2dFactory->CreateDevice(
					dxgiDevice.Get(),
					&m_d2dDevice
				);
			}

			void TexturePrinter::Run() noexcept
			{
				HRESULT hr = CreateDeviceContext();
				if (SUCCEEDED(hr))
				{
					size_t failuresCount = 5;
					do
					{
						hr = OnPrint();
						if (FAILED(hr))
						{
							Sleep(250);
							failuresCount--;
						}
					} while (failuresCount && D2DERR_RECREATE_TARGET == hr);
				}
				if (FAILED(hr))
					Audio::g_persistentAudio->PlayJingle("no"_cue);
			}

			HRESULT TexturePrinter::OnPrint() noexcept
			{
				HRESULT hr = S_OK;

				if (!m_d2dDevice)
					return E_FAIL;
				// Initialize printing-specific resources and prepare the
				// printing subsystem for a job.
				hr = InitializePrintJob();
				ComPtr<ID2D1DeviceContext> d2dContextForPrint;
				if (SUCCEEDED(hr))
				{
					// Create a D2D Device Context dedicated for the print job.
					hr = m_d2dDevice->CreateDeviceContext(
						D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
						&d2dContextForPrint
					);
				}
				if (SUCCEEDED(hr))
				{
					Renderer2D::PtrWICBitmap wicBitmap;

					const auto image = m_outputImage.GetImage(0, 0, 0);
					DirectX::Blob outputImage;
					if (FAILED(DirectX::SaveToWICMemory(
						*image,
						DirectX::WIC_FLAGS_NONE,
						GUID_ContainerFormatBmp,
						outputImage)))
						return E_FAIL;

					Renderer2D::g_factory2D->CreateWICBitmap(
						Kernel::SpanConstBytes(
							outputImage.GetBufferPointer(),
							outputImage.GetBufferSize()).GetComIStreamTemp().Get(),
						/*outputImage.GetBufferSize()).GetComIStreamOnCopy().Get(),*/
						wicBitmap);
					if (wicBitmap && !m_customBitmap)
						hr = d2dContextForPrint->CreateBitmapFromWicBitmap(wicBitmap.Get(), nullptr, &m_customBitmap);

					if (SUCCEEDED(hr))
					{
						ComPtr<ID2D1CommandList> commandList;

						hr = d2dContextForPrint->CreateCommandList(&commandList);

						// Create, draw, and add a Direct2D Command List for each page.
						if (SUCCEEDED(hr))
						{
							d2dContextForPrint->SetTarget(commandList.Get());
							hr = DrawToContext(d2dContextForPrint.Get());
							commandList->Close();
						}

						if (SUCCEEDED(hr))
							hr = m_printControl->AddPage(commandList.Get(), D2D1::SizeF(m_pageWidth, m_pageHeight), nullptr);
					}
				}
				// Send the job to the printing subsystem and discard
				// printing-specific resources.
				HRESULT hrFinal = FinalizePrintJob();

				if (SUCCEEDED(hr))
					hr = hrFinal;

				if (hr == D2DERR_RECREATE_TARGET)
					DiscardDeviceResources();

				return hr;
			}

			HRESULT TexturePrinter::DrawToContext(
				_In_ ID2D1DeviceContext* d2dContext
			) noexcept
			{
				HRESULT hr = S_OK;

				// Get the size of the destination context ("page").
				const D2D1_SIZE_F sourceSize{ m_pageWidth - 2 * DEFAULT_DPI, m_pageHeight - 2 * DEFAULT_DPI };
				assert(0 < sourceSize.height);
				assert(0 < sourceSize.width);
				const float otherRatio = sourceSize.width / sourceSize.height;
				const auto IsA4Wider = [otherRatio]() noexcept { return A4_LANDSCAPE_RATIO > otherRatio; };
				const D2D1_SIZE_F targetSize =
				{
					sourceSize.width * (IsA4Wider() ? 1.0f : A4_LANDSCAPE_RATIO / otherRatio),
					sourceSize.height * (IsA4Wider() ? A4_LANDSCAPE_RATIO / otherRatio : 1.0f)
				};

				// Compute the translation matrix that simulates scrolling or printing.
				const D2D1_MATRIX_3X2_F scrollTransform =
					D2D1::Matrix3x2F::Translation(
						(m_pageWidth - targetSize.width) / 2.0f, (m_pageHeight - targetSize.height) / 2.0f);

				d2dContext->BeginDraw();

				d2dContext->SetTransform(scrollTransform);

				d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));

				// Draw a bitmap in the upper-left corner of the window.
				d2dContext->DrawBitmap(
					m_customBitmap.Get(),
					D2D1::RectF(0.0f, 0.0f, targetSize.width, targetSize.height)
				);

				hr = d2dContext->EndDraw();

				return hr;
			}

			HRESULT TexturePrinter::GetPrintTicketFromDevmode(
				_In_ PCTSTR printerName,
				_In_reads_bytes_(devModesize) PDEVMODE devMode,
				WORD devModesize,
				_Out_ LPSTREAM* printTicketStream) noexcept
			{
				HRESULT hr = S_OK;
				HPTPROVIDER provider = nullptr;

				*printTicketStream = nullptr;

				// Allocate stream for print ticket.
				*printTicketStream = SHCreateMemStream(nullptr, 0u);
				if (nullptr == *printTicketStream)
					return E_OUTOFMEMORY;

				if (SUCCEEDED(hr))
					hr = PTOpenProvider(printerName, 1, &provider);

				// Get PrintTicket from DEVMODE.
				if (SUCCEEDED(hr))
					hr = PTConvertDevModeToPrintTicket(provider, devModesize, devMode, kPTJobScope, *printTicketStream);

				if (FAILED(hr) /* && *printTicketStream != nullptr*/)
					// Release printTicketStream if fails.
					SAFE_RELEASE(*printTicketStream);

				if (provider)
					PTCloseProvider(provider);

				return hr;
			}

			HRESULT TexturePrinter::InitializePrintJob() noexcept
			{
				HRESULT hr = S_OK;
				//wchar_t messageBuffer[512] = { 0 };

				// Bring up Print Dialog and receive user print settings.
				PRINTDLGEX printDialogEx{
					.lStructSize = sizeof(PRINTDLGEX),
					.hwndOwner = Monad::Globals::g_mainWindow,
					.Flags = PD_RETURNDEFAULT | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE,
					.nStartPage = START_PAGE_GENERAL };

				HRESULT hrPrintDialogEx = PrintDlgEx(&printDialogEx);

				if (FAILED(hrPrintDialogEx))
				{
					// Error occured.
					/*StringCchPrintf(
						messageBuffer,
						_countof(messageBuffer),
						L"Error 0x%4X occured during printer selection and/or setup.",
						hrPrintDialogEx
						);
					MessageBox(Win32ApplicationGeneric::GetHwnd(), messageBuffer, L"Message"s, MB_OK);*/
					return hrPrintDialogEx;
				}
				/*else if (printDialogEx.dwResultAction == PD_RESULT_APPLY)
				{
					// User clicks the Apply button and later clicks the Cancel button.
					// For simpicity, this sample skips print settings recording.
					hr = E_FAIL;
				}
				else if (printDialogEx.dwResultAction == PD_RESULT_CANCEL)
				{
					// User clicks the Cancel button.
					hr = E_FAIL;
				}*/

				// Retrieve DEVNAMES from print dialog.
				DEVNAMES* devNames = nullptr;
				if (SUCCEEDED(hr))
					if (printDialogEx.hDevNames != nullptr)
					{
						devNames = reinterpret_cast<DEVNAMES*>(GlobalLock(printDialogEx.hDevNames));
						if (devNames == nullptr)
							hr = HRESULT_FROM_WIN32(GetLastError());
					}
					else
						hr = E_HANDLE;

				// Retrieve user settings from print dialog.
				DEVMODE* devMode = nullptr;
				PCWSTR printerName = nullptr;
				if (SUCCEEDED(hr))
				{
					printerName = reinterpret_cast<PCWSTR>(devNames) + devNames->wDeviceOffset;

					if (printDialogEx.hDevMode != nullptr)
					{
						devMode = reinterpret_cast<DEVMODE*>(GlobalLock(printDialogEx.hDevMode));   // retrieve DevMode
						if (devMode)
						{
							m_devMode.reset(reinterpret_cast<DEVMODE*>(new (std::nothrow) uint8_t[devMode->dmSize + devMode->dmDriverExtra]));
							if (!m_devMode)
								return E_OUTOFMEMORY;
							memcpy(m_devMode.get(), devMode, static_cast<size_t>(devMode->dmSize) + static_cast<size_t>(devMode->dmDriverExtra));
							m_devMode->dmFields |= DM_ORIENTATION;
							m_devMode->dmOrientation = DMORIENT_LANDSCAPE;
							// Must check corresponding flags in devMode->dmFields
							// Convert 1/10 of a millimeter DEVMODE unit to 1/96 of inch D2D unit
							m_pageHeight = m_devMode->dmPaperWidth / 254.0f * DEFAULT_DPI;
							m_pageWidth = m_devMode->dmPaperLength / 254.0f * DEFAULT_DPI;
						}
						else
							hr = HRESULT_FROM_WIN32(GetLastError());
					}
					else
						hr = E_HANDLE;
				}

				// Convert DEVMODE to a job print ticket stream.
				if (SUCCEEDED(hr))
				{
					hr = GetPrintTicketFromDevmode(
						printerName,
						m_devMode.get(),
						m_devMode->dmSize + m_devMode->dmDriverExtra, // Size of DEVMODE in bytes, including private driver data.
						&m_jobPrintTicketStream
					);
				}

				// Create a factory for document print job.
				IPrintDocumentPackageTargetFactory* documentTargetFactory = nullptr;
				if (SUCCEEDED(hr))
					hr = ::CoCreateInstance(
						__uuidof(PrintDocumentPackageTargetFactory),
						nullptr,
						CLSCTX_INPROC_SERVER,
						IID_PPV_ARGS(&documentTargetFactory)
					);

				// Initialize the print subsystem and get a package target.
				if (SUCCEEDED(hr))
					hr = documentTargetFactory->CreateDocumentPackageTargetForPrintJob(
						printerName,                                // printer name
						RemoveEmojiIcon(Globals::g_shortAppName),    // job name
						nullptr,                                    // job output stream; when nullptr, send to printer
						m_jobPrintTicketStream.Get(),                     // job print ticket
						&m_documentTarget                           // result IPrintDocumentPackageTarget object
					);

				// Create a new print control linked to the package target.
				if (SUCCEEDED(hr))
					hr = m_d2dDevice->CreatePrintControl(
						Renderer2D::g_factory2D->m_iwicFactory.Get(),
						m_documentTarget.Get(),
						nullptr,
						&m_printControl
					);

				// Create and register a print job checker.
				if (SUCCEEDED(hr))
				{
					SAFE_RELEASE(m_printJobChecker);
					m_printJobChecker.reset(new (std::nothrow) D2DPrintJobChecker);
					hr = (m_printJobChecker != nullptr) ? S_OK : E_OUTOFMEMORY;
				}
				if (SUCCEEDED(hr))
					hr = m_printJobChecker->Initialize(m_documentTarget.Get());

				// Release resources.
				if (devMode)
				{
					GlobalUnlock(printDialogEx.hDevMode);
					devMode = nullptr;
				}
				if (devNames)
				{
					GlobalUnlock(printDialogEx.hDevNames);
					devNames = nullptr;
				}
				if (printDialogEx.hDevNames)
					GlobalFree(printDialogEx.hDevNames);
				if (printDialogEx.hDevMode)
					GlobalFree(printDialogEx.hDevMode);

				SAFE_RELEASE(documentTargetFactory);

				return hr;
			}

			HRESULT TexturePrinter::FinalizePrintJob() noexcept
			{
				// Send the print job to the print subsystem. (When this call
				// returns, we are safe to release printing resources.)
				HRESULT hr = S_OK;
				if (m_printControl)
					hr = m_printControl->Close();

				return hr;
			}

			// Discards device-specific resources which need to be recreated
			// when a Direct3D device is lost.
			void TexturePrinter::DiscardDeviceResources() noexcept
			{
				m_d2dDevice.Reset();
				m_customBitmap.Reset();
			}
		}

		TaskTexturePrinter::TaskTexturePrinter(Renderer::PtrResource rtResource) noexcept :
			m_rtResource(rtResource)
		{}

		void TaskTexturePrinter::Run()
		{
			DirectX::ScratchImage outputImage;
			THROW_EXC_D3D(DirectX::CaptureTexture(
				g_dxSample->m_commandQueue.Get(),
				m_rtResource.Get(),
				false,
				outputImage), L"Capture");
			m_rtResource.Reset();
			g_shaderSketchGeneric->ReleasePrintingLock();
			TexturePrinter{ std::move(outputImage) }.Run();
		}
	}
}