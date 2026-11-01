// Book.cpp : Definiuje punkt wejścia dla aplikacji.
//

// Platform
#include "framework.h"
#include <wrl.h>
#include <d3d12.h>

// Monad
#include "Book.h"
#include "System/Library.h"
#include "I18N/I18N.h"
#include "Renderer/HardwareAdapter.h"

// Platform
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d12.lib")

BOOL g_executeCore = TRUE;
// Indicates to hybrid graphics systems to prefer the discrete part by default.

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int RunDLL()
{
	struct LocalLibraryCore : Monad::System::LocalLibrary
	{
		LocalLibraryCore() :
			LocalLibrary(L"Core.dll")
		{}
		using LPMiniMain = INT(WINAPI*)(VOID);
		const LPMiniMain c_fnMiniMain = reinterpret_cast<LPMiniMain>(GetProcAddress("MiniMain"));
	} coreDLL;
	return coreDLL.c_fnMiniMain();
}

bool SupportsAVX2() noexcept
{
#if defined(_M_X64)
	// Tablica na cztery rejestry wyjściowe: EAX, EBX, ECX, EDX
	int cpuInfo[4] = { 0 };

	// MSVC (Windows)
	__cpuidex(cpuInfo, 7, 0);

	// Flaga AVX2 znajduje się w rejestrze EBX (indeks 1), na pozycji 5. bita
	return (cpuInfo[1] & (1 << 5)) != 0;
#else
	return true;
#endif
}

const Monad::I18N::I18NString g_fullAppName
{
	L"Kolorowanka „Magiczna Kredka”"_pl_PL,
	L"Раскраска «Волшебные Краски»"_ru_RU,
	L"The Enchanted Crayon Colouring Book"_en_GB,
	L"The Enchanted Crayon Coloring Book"_en_US
};

void MsgBoxD3D()
{
	const Monad::I18N::I18NString CAPS_FAILED
	{
		L"Karta graficzna lub jej sterowniki nie spełniają minimalnych wymagań"_pl_PL,
		L"Видеокарта или драйвер не отвечают минимальным требованиям"_ru_RU,
		L"Graphics adapter or its driver do not meet minimal software requirements"_en_GB,
		L"The graphics adapter or its driver do not meet minimal software requirements"_en_US
	};
	MessageBox(0, CAPS_FAILED.ToLPCWSTR(), g_fullAppName.ToLPCWSTR(), 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,
	_In_ int
)
{
	if (!SupportsAVX2())
	{
		Monad::I18N::I18NString	g_noAVX2
		{
			L"Mikroprocesor nie spełnia minimalnych wymagań (AVX2)"_pl_PL,
			L"Микропроцессор не отвечает минимальным требованиям (AVX2)"_ru_RU,
			L"Microprocessor does not meet minimal software requirements (AVX2)"_en_GB,
			L"The microprocessor does not meet minimal software requirements (AVX2)"_en_US
		};
		MessageBox(0, g_noAVX2.ToLPCWSTR(), g_fullAppName.ToLPCWSTR(), 0);
		return 1;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
#if defined _DEBUG
	constexpr uint32_t DXGI_FACTORY_FLAGS = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr uint32_t DXGI_FACTORY_FLAGS = 0u;
#endif

	if (FAILED(CreateDXGIFactory2(
		DXGI_FACTORY_FLAGS,
		IID_PPV_ARGS(&m_dxgiFactory)))
		)
	{
		MsgBoxD3D();
		return 1;
	}
	else
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
		Monad::Renderer::GetHardwareAdapter(m_dxgiFactory.Get(), &hardwareAdapter, true);

		if (FAILED(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), 
			nullptr))
			)
		{
			MsgBoxD3D();
			return 1;
		}
	}
	/*do
	{
		std::jthread D3DThread(RunDLL);
		D3DThread.join();
	} while (FALSE != g_executeCore);*/
	return RunDLL();
}