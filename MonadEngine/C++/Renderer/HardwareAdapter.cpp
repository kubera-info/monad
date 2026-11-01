#include "stdafx.h"
#include "HardwareAdapter.h"
#include <wrl.h>

namespace Monad::Renderer
{
	std::wstring GetHardwareAdapter(
		IDXGIFactory6* factory,
		IDXGIAdapter1** pAdapter,
		bool requestHighPerformanceAdapter
	)
	{
		std::wstring adapterDesc;

		*pAdapter = nullptr;

		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

		Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (
				UINT adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true
					? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
					: DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter))
				);
				++adapterIndex
				)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(
					adapter.Get(),
					D3D_FEATURE_LEVEL_12_0,
					_uuidof(ID3D12Device),
					nullptr))
					)
				{
					adapterDesc = desc.Description;
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(
				adapterIndex,
				&adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(
					adapter.Get(),
					D3D_FEATURE_LEVEL_12_0,
					_uuidof(ID3D12Device),
					nullptr))
					)
				{
					adapterDesc = desc.Description;
					break;
				}
			}
		}
		*pAdapter = adapter.Detach();
		return adapterDesc;
	}
}