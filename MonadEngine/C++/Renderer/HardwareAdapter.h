#pragma once

// Platform
#include <dxgi1_6.h>
#include <d3d12.h>

// STD
#include <string>

namespace Monad::Renderer
{
	/// <summary>
	/// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	/// If no such adapter can be found, *ppAdapter will be set to nullptr.
	/// </summary>
	/// <param name="factory">DXGI Factory</param>
	/// <param name="pAdapter">Output Adapter/nullptr</param>
	/// <param name="requestHighPerformanceAdapter">High Performance Adapter?</param>
	/// <returns>GPU Name</returns>
	std::wstring GetHardwareAdapter(
		IDXGIFactory6* factory,
		IDXGIAdapter1** pAdapter,
		bool requestHighPerformanceAdapter
	);
}
