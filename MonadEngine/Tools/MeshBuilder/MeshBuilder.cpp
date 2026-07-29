// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#define STRICT
// Use the C++ standard templated min/max
#define NOMINMAX
// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define WIN32_LEAN_AND_MEAN

// #define DXUT_AUTOLIB to automatically include the libs needed for DXUT 
#pragma comment( lib, "d3d9.lib" )
#if defined(_DEBUG)
#	pragma comment( lib, "d3dx9d.lib" )
#else
#	pragma comment( lib, "d3dx9.lib" )
#endif

#include <d3dx9.h>
#include "HRCore.h"
#include "PaintingCore.h"
#include "Wallpaper.h"

using Microsoft::WRL::ComPtr;

constexpr LPCWCHAR g_wndClass = L"D3DClass";

HRESULT g_hr = S_OK;

ComPtr<IDirect3DDevice9> pD3DDevice;

void ReportError(const wchar_t* errorDesc)
{
	wprintf(L"\nError: %s; %x\n", errorDesc, g_hr);
	MessageBeep(MB_ICONERROR);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and FindArtByFilename what code to run for the message given
	switch (message)
	{
	case WM_CREATE:
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return 0;
	}
	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
		UnregisterClass(g_wndClass, NULL);
		return 0;
	}
	// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	}

	// FileHandle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, wchar_t*, int)
{
	// Enable run-time memory check for debug builds.
#if defined _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	HWND hWnd(NULL);
	// this struct holds information for the window class
	const WNDCLASSEX WndClass
	{
	.cbSize = sizeof WNDCLASSEX,
	.style = CS_HREDRAW | CS_VREDRAW,
	.lpfnWndProc = WindowProc,
	.hInstance = hInstance,
	.hCursor = LoadCursor(NULL, IDC_ARROW),
	.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW),
	.lpszClassName = g_wndClass
	};
	// register the window class
	RegisterClassEx(&WndClass);

	// create the window and use the result as the handle
	if (NULL == (hWnd = CreateWindowEx(NULL,
		g_wndClass,    // name of the window class
		L"Mesh Builder",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		640,    // width of the window
		480,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL)))    // used with multiple windows, NULL
	{
		ReportError(L"D3D window can not be created");
		return 2;
	}

	ComPtr<IDirect3D9> pD3D;
	if (nullptr == ((pD3D = Direct3DCreate9(D3D_SDK_VERSION))))
	{
		ReportError(L"D3D window can not be created");
		return 3;
	}

	D3DPRESENT_PARAMETERS d3dpp{
	.SwapEffect = D3DSWAPEFFECT_COPY,
	.Windowed = TRUE
	};
	if (FAILED((g_hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &pD3DDevice))))
	{
		ReportError(L"Device can not be created");
		return 4;
	}
	//MeshCreatorPainting DialogGeneric;
	//MeshHRCreator HR;
	//MeshCreatorWallpaper Wallpaper;
	MSG msg{};

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ERROR_SUCCESS;
}