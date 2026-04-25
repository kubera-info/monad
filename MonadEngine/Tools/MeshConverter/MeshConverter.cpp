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
#if defined _DEBUG
#	pragma comment( lib, "d3dx9d.lib" )
#else
#	pragma comment( lib, "d3dx9.lib" )
#endif

// Platform
#include <d3d12.h>
#include <d3dx9.h>
#include <wrl.h>
// STD
#include <cstdint>
#include <memory>
// External
#include "../../../Modules/meshoptimizer/src/meshoptimizer.h"
// Monad
#include "../../C++/Kernel/BytesVector.h"
#include "../../C++/Renderer/MeshOpt.h"

using Microsoft::WRL::ComPtr;

constexpr const wchar_t* g_wndClass = L"D3DClass";

HRESULT g_hr = S_OK;

BOOL WriteFile(
	_In_ HANDLE hFile,
	_In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToWrite
)
{
	return WriteFile(
		hFile,
		lpBuffer,
		nNumberOfBytesToWrite,
		nullptr,
		nullptr
	);
}

void ReportError(
	const wchar_t* errorDesc
)
{
	wprintf(L"\nError: %s; %x\n", errorDesc, g_hr);
	MessageBeep(MB_ICONERROR);
}

LRESULT CALLBACK WindowProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
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
	return DefWindowProc(
		hWnd,
		message,
		wParam,
		lParam
	);
}

INT WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE,
	wchar_t*,
	int
)
{
	// Enable run-time memory check for debug builds.
#if defined _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	if (3 != __argc)
	{
		ReportError(L"Wrong count of parameters");
		return EXIT_FAILURE;
	}

	int successAtExit = EXIT_FAILURE;

	// this struct holds information for the window class
	const WNDCLASSEX WndClass
	{
	.cbSize = sizeof WNDCLASSEX,
	.style = CS_HREDRAW | CS_VREDRAW,
	.lpfnWndProc = WindowProc,
	.hInstance = hInstance,
	.hCursor = NULL,
	.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW),
	.lpszClassName = g_wndClass
	};
	// register the window class
	RegisterClassEx(&WndClass);

	// create the window and use the result as the handle
	const HWND hWnd = CreateWindowEx(NULL,
		g_wndClass,    // name of the window class
		L"Mesh Converter",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		320,    // width of the window
		200,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL
	if (NULL == hWnd)
	{
		ReportError(L"D3D window cannot be created");
		return EXIT_FAILURE;
	}

	ComPtr<IDirect3D9Ex> pD3DEx;
	if (FAILED((g_hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3DEx))))
	{
		ReportError(L"D3D window cannot be created");
		return EXIT_FAILURE;
	}

	D3DPRESENT_PARAMETERS d3dpp{
	.SwapEffect = D3DSWAPEFFECT_COPY,
	.Windowed = TRUE
	};
	ComPtr<IDirect3DDevice9> pD3DDevice;
	if (FAILED((g_hr = pD3DEx->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&pD3DDevice)))
		)
	{
		ReportError(L"Device cannot be created");
		return EXIT_FAILURE;
	}
	ComPtr<ID3DXMesh> mesh;
	if (SUCCEEDED((g_hr = D3DXLoadMeshFromX(
		__wargv[1],
		D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM,
		pD3DDevice.Get(),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		&mesh)))) [[likely]]
	{
		LPVOID indexBuffer(nullptr), vertexBuffer(nullptr);
		const UINT
			indicesCount(mesh->GetNumFaces() * 3),
			verticesCount = mesh->GetNumVertices(),
			vertexSize = mesh->GetNumBytesPerVertex();
		if (SUCCEEDED(mesh->LockIndexBuffer(D3DLOCK_READONLY, &indexBuffer))
			&& SUCCEEDED(mesh->LockVertexBuffer(D3DLOCK_READONLY, &vertexBuffer))) [[likely]]
		{
			if (indicesCount <= D3D12_16BIT_INDEX_STRIP_CUT_VALUE) [[likely]]
			{
				std::unique_ptr<uint16_t[]> indexBuffer2;
				indexBuffer2 = std::make_unique_for_overwrite<WORD[]>(indicesCount);
				for (size_t index = 0; index < indicesCount; ++index)
					indexBuffer2[index] = LOWORD((reinterpret_cast<UINT*>(indexBuffer))[index]);
				Monad::Kernel::SpanIndices16 indexBuffer2Span{ indexBuffer2.get(), indicesCount };
				const auto optimizedMesh = Monad::Renderer::OptimizeMesh(
					indexBuffer2Span,
					{ vertexBuffer, vertexSize, verticesCount }
				);
				if (HANDLE f = CreateFile2(__wargv[2], GENERIC_WRITE, 0, CREATE_ALWAYS, nullptr);
					f != INVALID_HANDLE_VALUE
					&& WriteFile(f, &vertexSize, sizeof vertexSize)
					&& WriteFile(f, &indicesCount, sizeof indicesCount)
					&& WriteFile(f,
						optimizedMesh.m_indices.data(),
						static_cast<uint32_t> (optimizedMesh.m_indices.size() * sizeof(uint16_t))
					)
					&& WriteFile(
						f,
						optimizedMesh.m_vertices.data(),
						static_cast<uint32_t> (optimizedMesh.m_vertices.size() * vertexSize)
					)
					&& CloseHandle(f)
					) [[likely]]
				{
					successAtExit = ERROR_SUCCESS;
					wprintf(
						L"Mesh(16bit) %s has been created %u (indices), %u (verts), %u (size of vert.).\n",
						__wargv[2],
						indicesCount,
						verticesCount,
						vertexSize
					);
				}
			}
			else
			{
				Monad::Kernel::SpanIndices32 indexBufferSpan{ static_cast<uint32_t*> (indexBuffer), indicesCount };
				const auto optimizedMesh = Monad::Renderer::OptimizeMesh(
					indexBufferSpan,
					{ vertexBuffer, vertexSize, verticesCount }
				);
				if (HANDLE f = CreateFile2(__wargv[2], GENERIC_WRITE, 0, CREATE_ALWAYS, nullptr);
					f != INVALID_HANDLE_VALUE
					&& WriteFile(f, &vertexSize, sizeof vertexSize)
					&& WriteFile(f, &indicesCount, sizeof indicesCount)
					&& WriteFile(f,
						optimizedMesh.m_indices.data(),
						static_cast<uint32_t> (optimizedMesh.m_indices.size() * sizeof(uint32_t)
							)
					)
					&& WriteFile(
						f,
						optimizedMesh.m_vertices.data(),
						static_cast<uint32_t> (optimizedMesh.m_vertices.size() * vertexSize)
					)
					&& CloseHandle(f)
					) [[likely]]
				{
					successAtExit = ERROR_SUCCESS;
					wprintf(
						L"Mesh(32bit) %s has been created %u (indices), %u (verts), %u (size of vert.).\n",
						__wargv[2],
						indicesCount,
						verticesCount,
						vertexSize
					);
				}
			}
		}
		mesh->UnlockVertexBuffer();
		mesh->UnlockIndexBuffer();
	}
	else [[unlikely]]
	{
		ReportError(L"Mesh cannot be created");
		return EXIT_FAILURE;
	}
	MSG msg{};

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (ERROR_SUCCESS != successAtExit)
	{
		ReportError(L"Conversion error");
		return EXIT_FAILURE;
	}
	else
		return ERROR_SUCCESS;
}