// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "WindowSat.h"

// Monad
#include "Globals/__MonadGlobals.h"
#include "Exceptions/Exceptions.h"
#include "Files/FileManager.h"
#include "GUI/__MonadGUI.h"
#include "Kernel/Dummy.h"
#include "Kernel/Format.h"
#include "Kernel/Templates.h"
#include "Monitor.h"
#include "Threads/Thread.h"
#include "Math/Random.h"
#include "Tools/Convert.h"
#include "Message.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace std::string_literals;

namespace Monad
{
	using namespace Input;
	using namespace Kernel;
	using namespace Math;

#define WM_APP_CLOSE WM_APP

	namespace Wnd
	{
		namespace
		{
			inline void ProcessMsg(
				const HWND pHWND
			) noexcept
			{
				MSG msg{};
				BOOL bRet;

				while ((bRet = GetMessage(&msg, pHWND, 0, 0)) != 0)
				{
					if (-1 == bRet) [[unlikely]]
						break;
					// handle the error and possibly exit
					else [[likely]]
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}

			ExpectedLResult ProcessWM_APP_CLOSE(
				HWND hWnd,
				UINT,
				WPARAM,
				LPARAM,
				LPVOID&
			)
			{
				DestroyWindow(hWnd);
				return PROCESSING_OK;
			}

			ExpectedLResult ProcessWM_CLOSE(
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam,
				LPARAM lParam,
				LPVOID& context
			)
			{
				Exceptions::RegisterWM_CLOSE();
				return ProcessWM_APP_CLOSE(
					hWnd,
					uMsg,
					wParam,
					lParam,
					context
				);
			}

			ExpectedLResult ProcessWM_ERASEBKGND(
				HWND,
				UINT,
				WPARAM,
				LPARAM,
				LPVOID&
			)
			{
				return PROCESSING_OK;
			}

			ExpectedLResult ProcessWM_NCCREATE(
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam,
				LPARAM lParam,
				LPVOID& context
			);

			ExpectedLResult ProcessWM_PAINT(
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam,
				LPARAM lParam,
				LPVOID& context
			);

			ExpectedLResult ProcessWM_SETCURSOR(
				HWND hWnd,
				UINT uMsg,
				WPARAM wParam,
				LPARAM lParam,
				LPVOID& context
			);

			MapForMessages g_forSatMessage
			{
				make_pair(
					WM_APP_CLOSE,
					ProcessWM_APP_CLOSE),
				make_pair(
					WM_CLOSE,
					ProcessWM_CLOSE),
				make_pair(
					WM_ERASEBKGND,
					ProcessWM_ERASEBKGND),
				make_pair(
					WM_NCCREATE,
					ProcessWM_NCCREATE),
				make_pair(
					WM_PAINT,
					ProcessWM_PAINT),
				make_pair(
					WM_SETCURSOR,
					ProcessWM_SETCURSOR)
			};

#pragma region Satellite
			/// <summary>
			/// This class forces to call the code, before creating attributes of the ThreadSatellite's.
			/// </summary>
			interface ThreadWnd abstract
			{
				RECT m_wndRect;
				HWND m_hWnd;
			};
			/// <summary>
			/// One satellite
			/// The message loop is in a new thread
			/// A window shows a picture in Direct2D
			/// </summary>
			struct ThreadSatellite final : ThreadWnd, Threads::Thread
			{
				ThreadSatellite(
					const RECT& monitorRect
				) :
					ThreadWnd{
					monitorRect,
					nullptr
				},
					Thread{ L"Satellite" }
				{}

				~ThreadSatellite()
				{
					PostMessage(m_hWnd, WM_APP_CLOSE, 0, 0);
					join();
				}
				OPER_DEL(ThreadSatellite);

				// inline operator const RECT& () const noexcept { return m_wndRect; }

				static LRESULT CALLBACK WindowProc(
					HWND hWnd,
					UINT uMsg,
					WPARAM wParam,
					LPARAM lParam
				) noexcept
				{
					try
					{
						thread_local LPVOID windowContext = nullptr;
						if (auto lres = OnCommonMessages2(
							{ g_forSatMessage, g_forMessage },
							hWnd,
							uMsg,
							wParam,
							lParam,
							windowContext
						);
							lres.has_value())
							return *lres;
						/*case WM_SIZE:
						{
							if (pThis.p->m_pRT)
							{
								const D2D1_SIZE_U size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));

								// If we couldn't resize, release the device and we'll recreate it
								// during the next render pass.
								if (FAILED(pThis.p->m_pRT->Resize(size)))
								{
									pThis.p->DiscardDeviceResources();
								}
							}
							// break;
						}
						[[fallthrough]];
						case WM_MOVE:
						{
							GetWindowRect(hWnd, &pThis.p->m_wndRect);
							break;
						}*/
					}
					MONAD_CATCH;
					return DefWindowProc(
						hWnd,
						uMsg,
						wParam,
						lParam
					);
				}

				inline void DiscardDeviceResources()
				{
					m_pRT.Reset();
					m_pD2DBitmap.Reset();
				}

				/*friend bool operator==(const RECT& pLeft, const ThreadSatellite& pRight) noexcept
				{
					return std::strong_ordering::equivalent == Kernel::operator<=>(pLeft, pRight.m_wndRect);
				}*/

				void OnPaint()
				{
					PAINTSTRUCT ps{};

					if (BeginPaint(m_hWnd, &ps))
					{
						try
						{
							// Create render target if not yet created
							CreateDeviceResources();
							if (m_pRT)
								if (!(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
								{
									m_pRT->BeginDraw();
									const D2D1_SIZE_F rtSize = m_pRT->GetSize();

									thread_local const float
										widthScale = g_random.GetFlip(),
										heightScale = g_random.GetFlip();
									m_pRT->SetTransform(
										D2D1::Matrix3x2F::Scale(
											widthScale,
											heightScale,
											{ rtSize.width * GUI::CENTER,
											rtSize.height * GUI::CENTER })
									);

									// Clear the background 
									// m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

									// D2DBitmap may have been released due to device loss. 
									// If so, re-create it from the source bitmap
									if (!m_pD2DBitmap)
										m_pRT->CreateBitmapFromWicBitmap(
											m_wicBitmap.Get(),
											nullptr,
											&m_pD2DBitmap
										);

									// Draws an image and scales it to the current window size
									if (m_pD2DBitmap)
										m_pRT->DrawBitmap(
											m_pD2DBitmap.Get(),
											{
												0.0f,
												0.0f,
												rtSize.width,
												rtSize.height }
												);

									// In case of device loss, discard D2D render target and D2DBitmap
									// They will be re-create in the next rendering pass
									if (m_pRT->EndDraw() == D2DERR_RECREATE_TARGET)
									{
										DiscardDeviceResources();
										// Force a re-render
										InvalidateRect(m_hWnd, nullptr, FALSE);
									}
								}
						}
						MONAD_CATCH;

						EndPaint(m_hWnd, &ps);
					}
				}

				void CreateDeviceResources() noexcept
				{
					if (!m_pRT)
					{
						m_cursorControl.SetNewCurrentCursor(CURSOR_WAIT);
						// Create a D2D render target properties
						D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();

						// Set the DPI to be the default system DPI to allow direct mapping
						// between image pixels and desktop pixels in different system DPI settings
						renderTargetProperties.dpiX = renderTargetProperties.dpiY = DEFAULT_DPI;

						// Create a D2D render target
						const D2D1_SIZE_U size = D2D1::SizeU(
							m_wndRect.right - m_wndRect.left,
							m_wndRect.bottom - m_wndRect.top
						);

						if (SUCCEEDED(m_d2dFactory->CreateHwndRenderTarget(
							renderTargetProperties,
							D2D1::HwndRenderTargetProperties(m_hWnd, size),
							&m_pRT
						)))
							m_cursorControl.SetNewCurrentCursor(CURSOR_NO);
					}
				}

				void Routine()
				{
					{
						Files::TaskFileHolder loader(
							Files::g_fileSearcherGeneric,
							g_managerSatellites->m_folderSat / (
								Tools::UTF8ToUnicode(g_managerSatellites->m_blobJpgNames.GetRandomString() + ".monad-lob")));
						loader.Run();
						g_managerSatellites->m_2D.CreateWICBitmap(loader.GetComIStreamTemp().Get(), m_wicBitmap);
					}
#if defined _DEBUG
					constexpr D2D1_FACTORY_OPTIONS debugLevel{ D2D1_DEBUG_LEVEL_INFORMATION };
#else
					constexpr D2D1_FACTORY_OPTIONS debugLevel{ D2D1_DEBUG_LEVEL_NONE };
#endif
					THROW_EXC_IFFAILED(Exceptions::NotInitialized, D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory2), &debugLevel, &m_d2dFactory), L"2D Init");

					SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

					ProcessMsg((m_hWnd = g_managerSatellites->m_satelliteWindowClass.WindowFactory(
						m_wndRect,
						this,
						Globals::g_fullAppName)));
				}

				Renderer2D::PtrWICBitmap m_wicBitmap;
				ComPtr<ID2D1Factory2> m_d2dFactory;
				ComPtr<ID2D1HwndRenderTarget> m_pRT;
				ComPtr<ID2D1Bitmap> m_pD2DBitmap;
				CursorControl m_cursorControl;
			};
#pragma endregion
		}

#pragma region ManagerSatellites
		struct ManagerSatellites::SetSatellites : UnorderedMap<RECT, ThreadSatellite>
		{
			using UnorderedMap<RECT, ThreadSatellite>::UnorderedMap;
		};

		void ManagerSatellites::SetSatelliteWindows()
		{
			if (const auto monitors = GetNonPrimaryMonitors(); !monitors.empty())
			{
				if (!m_satellites)
					m_satellites = std::make_unique<SetSatellites>();
				std::erase_if(*m_satellites, [&monitors](const auto& sat)
					{
						return !monitors.contains(sat.second.m_wndRect);
					}
				);
				for (const auto& monitorRect : monitors)
					m_satellites->try_emplace(monitorRect, monitorRect);
			}
			else
				m_satellites.reset();
		}

		void ManagerSatellites::SetTitle() const noexcept
		{
			if (m_satellites)
				for (auto& satellite : *m_satellites)
					SetWindowTitle(satellite.second.m_hWnd);
		}

		ManagerSatellites::ManagerSatellites(
			Renderer2D::Factory2D& factory2D
		) :
			c_me(this, g_managerSatellites),
			m_2D(factory2D),
			m_folderSat{ Kernel::DummyParameter{}, L"Sat" },
			m_blobJpgNames{ "background"s, "background1"s, "background2"s, "background3"s }
		{
			SetSatelliteWindows();
		}

		ManagerSatellites::~ManagerSatellites()
		{}

		extern ManagerSatellites* g_managerSatellites = nullptr;
#pragma endregion

#pragma region ManagerSatellites::WindowClassSatellite
		ManagerSatellites::WindowClassSatellite::WindowClassSatellite() :
			WindowClass{ L"WndClassSat", ThreadSatellite::WindowProc }
		{}

		HWND ManagerSatellites::WindowClassSatellite::WindowFactory(
			const RECT& wndRect,
			PVOID createWindowParameter,
			const I18N::I18NString& appTitle) const
		{
			return WindowFactoryGeneric(
				WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
				wndRect,
				createWindowParameter,
				/* WS_MAXIMIZE | */ WS_POPUP | WS_VISIBLE,
				appTitle);
		}
#pragma endregion

		namespace
		{
			ExpectedLResult ProcessWM_NCCREATE(
				HWND hWnd,
				UINT,
				WPARAM,
				LPARAM lParam,
				LPVOID& context
			)
			{
				context = reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams;
				assert(context);
				SetLastError(ERROR_SUCCESS);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR&>(context));
				THROW_EXC_ONERROR(Exceptions::InvalidClass, GetLastError(), L"Set Window Long Ptr");
				return PROCESSING_TRUE;
			}

			ExpectedLResult ProcessWM_PAINT(
				HWND,
				UINT,
				WPARAM,
				LPARAM,
				LPVOID& context
			)
			{
				static_cast<ThreadSatellite*> (context)->OnPaint();
				return PROCESSING_OK;
			}

			ExpectedLResult ProcessWM_SETCURSOR(
				HWND,
				UINT,
				WPARAM,
				LPARAM,
				LPVOID& context
			)
			{
				static_cast<ThreadSatellite*> (context)->m_cursorControl.SetCurrentCursor();
				return PROCESSING_TRUE;
			}
		}
	}
}