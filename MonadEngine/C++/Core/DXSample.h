// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <dxgi1_6.h>
#include <DirectXMath.h>
#if defined _DEBUG
#	include <dxgidebug.h>
#endif
// STD
#include <initializer_list>
// Monad
#include "__MonadCore.h"
#include "Modules/Xbox-ATG-Samples/Kits/CPUTK/OSLockable.h"
#include "Kernel/Templates.h"
#include "Renderer/CommandList.h"
#include "Renderer/ConstantBuffers.h"
#include "Renderer/HDR.h"
#include "Renderer/Heap.h"
#include "Renderer/Matrices.h"
#include "Renderer/Shader0.h"
#include "Renderer/Targets.h"
#include "Renderer/Combine.h"
#include "Renderer/InputLayout.h"

/// <summary>
/// Ensures that specific initialization code is executed prior to creating DXSample attribute objects.
/// </summary>
interface DXSampleGeneric abstract
{
	DXSampleGeneric() noexcept;
	virtual ~DXSampleGeneric() = default;
};

/// <summary>
/// Number of texture resources currently available or loaded.
/// </summary>
extern uint32_t g_shaderResourceViewCount;

interface DXSample abstract : DXSampleGeneric, Monad::Renderer::Combine
{
	const Monad::Kernel::Me<DXSample> c_me;
	const Monad::Renderer::InputLayout::MapInputLayouts c_inputLayouts;
	using MapTechniques = Monad::Kernel::FlatMapString<Monad::Renderer::Technique>;
	MapTechniques m_techniques;
	using InitializerListTechniques = std::initializer_list<MapTechniques::value_type>;

	DXSample(
		Monad::Renderer::InputLayout::MapInputLayouts inputLayouts,
		const Counts counts,
		const uint32_t sampleCount,
		Monad::Renderer::MapFX::InitializerListFXes fxCollection,
		InitializerListTechniques techniques,
		Monad::Renderer::CB::MapConstantBufferManager::InitializerListConstantBufferManager cbTypeGenerics
	);
	OPER_DEL(DXSample);
	~DXSample() override;

	void OnFrameRenderAll(
		const Monad::Renderer::STAGE_MODES mode
	);
	static inline uint32_t GetWidth() noexcept { return m_width; }
	static inline uint32_t GetHeight() noexcept { return m_height; }
	// inline bool GetTearingSupport() const noexcept { return m_tearingSupport; }

	static inline void UpdateForSizeChange(
		uint32_t clientWidth,
		uint32_t clientHeight
	) noexcept
	{
		m_width = clientWidth;
		m_height = clientHeight;
	}

#if defined _DEBUG
	static void ReportLiveObjects();
	void ReportPresent();
	std::wstring LogInfoQue();

	Microsoft::WRL::ComPtr<ID3D12InfoQueue> m_d3dInfoQueue;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_dxgiInfoQueue;
#endif

	void CreateOneRootSignature(
#if defined _DEBUG
		const std::wstring& name,
#endif
		const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc,
		Monad::Renderer::PtrRootSignature& rootSignature
	);

protected:
	/// <summary>
	/// Determines whether tearing support is available for fullscreen borderless windows.
	/// </summary>
	void CheckTearingSupport() noexcept;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	void OnBackBuffer();

	/// <summary>
	/// Viewport dimensions.
	/// Whether or not tearing is available for fullscreen borderless windowed mode.
	/// </summary>
	static uint32_t m_width, m_height;
	bool m_tearingSupport = false;
#pragma region HDR

public:
	Monad::Renderer::HDR::SWAP_CHAIN_BIT_DEPTH m_currentSwapChainBitDepth;

protected:
	Monad::Renderer::FormatAndClearRenderTarget
		m_renderTargetViewDesc,
		m_renderTargetMSAAViewDesc;
	// Color.
	bool m_hdrSupport = false;
	bool m_enableST2084 = false;
	float m_referenceWhiteNits = 80.0f;    // The reference brightness level of the display.
	float m_peakBrightnessNits = 1000.0f;  // The peak brightness level of the display.
	DXGI_COLOR_SPACE_TYPE m_currentSwapChainColorSpace;

	/// <summary>
	/// DirectX supports two combinations of swapchain pixel formats and colorspaces for HDR content.
	/// Option: R10G10B10A2 + DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020
	/// Calling this function to ensure the correct color space for the different pixel formats.
	/// </summary>
	/// <param name="d">swapchain pixel formats and colorspaces</param>
	/// <param name="enableST2084">Enable ST2084</param>
	void EnsureSwapChainColorSpace(
		Monad::Renderer::HDR::SWAP_CHAIN_BIT_DEPTH swapChainBitDepth,
		bool enableST2084
	);
	/// <summary>
	/// Set HDR meta data for output display to master the content and the luminance values of the content.
	/// An app should estimate and set appropriate metadata based on its contents.
	/// For demo purpose, we simply made up a few set of metadata for you to experience the effect of appling meta data.
	/// Please see details in https://msdn.microsoft.com/en-us/library/windows/desktop/mt732700(v=tableID.85).aspx.
	/// </summary>
	void SetHDRMetaData(
		float maxOutputNits = 1000.0f,
		float minOutputNits = 0.001f,
		float maxCLL = 2000.0f,
		float maxFALL = 500.0f
	);

public:
	const Monad::Renderer::HDR::HDRProfile& GetCurrentHDRConfig() const noexcept;
#pragma endregion

	/// <summary>
	/// Total number of Render Targets and the number of its Shader Resources (the same value).
	/// </summary>
	uint32_t m_countOfRT = 0;
	/// <summary>
	/// Total number of Depth-Stencils and the number of its Shader Resources (the same value).
	/// </summary>
	uint32_t m_countOfDS = 0;
	std::atomic_uint32_t m_cbSrCurrentID, m_rtCurrentID, m_dsCurrentID;
	/// <summary>
	/// Total number of Constant Blocks and Shader Resources.
	/// </summary>
	const uint32_t c_countOfCBSR;
	Monad::Renderer::Matrices m_matrices;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
	Monad::Renderer::PtrD3D12Device m_device;
	struct Frame
	{
		Monad::Renderer::PtrResource m_renderTargetRes;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		uint64_t m_fenceValue = 0;
	};
	Monad::Renderer::GraphicsCommandList1 m_commandList;
	Monad::Kernel::ArrayFrames<Frame> m_frames;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Monad::Renderer::ShaderConfigGeneric::MapRootSignatures m_rootSignatures;
	Monad::Renderer::PtrHeapCombine m_rtvHeap, m_dsvHeap, m_cbvSrvHeap;

#pragma region MSAA resources
	const uint32_t c_sampleCount;

protected:
	Monad::Renderer::RenderTargetDepthStencil m_msaaTargets;
#pragma endregion

public:
	void OnD3DCreateAllocators();
	void OnD3DCreateCommandList();
	void OnD3DReset();
	/// <summary>
	/// Prepare to render the next frame.
	/// </summary>
	void MoveToNextFrame();
	/// <summary>
	/// Load the rendering pipeline dependencies.
	/// </summary>
	void LoadPipeline();
	void LoadAssets();
	/// <summary>
	/// Set up the screen viewport and scissor rect to match the current window size and scene rendering resolution.
	/// Set up appropriate views for the intermediate render target.
	/// </summary>
	void LoadSizeDependentResources();
	/// <summary>
	/// Wait for pending GPU work to complete.
	/// </summary>
	void WaitForGpu() noexcept;
	bool OnBoth();

	/// <summary>
	// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	// If no such adapter can be found, *ppAdapter will be set to nullptr.
	/// </summary>
	/// <param name="factory">DXGI Factory</param>
	/// <param name="pAdapter">Output Adapter/nullptr</param>
	/// <param name="requestHighPerformanceAdapter">High Performance Adapter?</param>
	void GetHardwareAdapter(
		IDXGIFactory6* factory,
		IDXGIAdapter1** pAdapter,
		bool requestHighPerformanceAdapter = true
	);

	virtual void Repo() {}

#if defined MONAD_SHADOW
	Monad::Renderer::DepthStencil m_shadows;
#endif
	Monad::Renderer::CB::MapConstantBufferManager m_constantBufferManager;
	Monad::Renderer::MapFX m_fxCollection;
} extern* g_dxSample;

#define g_d3d12Core (static_cast<D3D12Core*>(g_dxSample))
