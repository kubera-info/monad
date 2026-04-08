// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#ifndef STRICT
#	define STRICT
#endif
// Use the C++ standard templated min/max
#ifndef NOMINMAX
#	define NOMINMAX
#endif

// DirectX apps don't need GDI
#define NODRAWTEXT
//#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

//#define WIN32_LEAN_AND_MEAN

// Platform
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <shlwapi.h>
/*#if defined MONAD_PIX
#	include <pix3.h>
#	pragma comment(lib, "..\\..\\packages\\WinPixEventRuntime.1.0.230302001\\bin\\x64\\WinPixEventRuntime.lib")
#endif*/
// STD
#include <assert.h>
#include <string>
#include <memory>

#define V_RETURN(x)		{ if (const HRESULT hr = (x); FAILED(hr)) return hr; }

#define SAFE_RELEASE(p)	{ if (p) { (p)->Release(); (p) = nullptr; } }

extern HINSTANCE g_hInstance;