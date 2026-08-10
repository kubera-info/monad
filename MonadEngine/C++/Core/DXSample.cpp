//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "DXSample.h"
// Platform
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d12.lib")
// STD
#include <numeric>
#include <sstream>
#include <utility>
// Monad
#include "Core/DXSampleHelper.h"
#include "Dev/Coding.h"
#include "Files/FileManager.h"
#include "Globals/__MonadGlobals.h"
#include "Kernel/Events.h"
#include "Renderer/__MonadRenderer.h"
#include "Time/__MonadTime.h"
#include "Time/Timer.h"
#include "Tools/Convert.h"
#include "Win32ApplicationGeneric.h"
#include "Wnd/Monitor.h"
#include "Repository/Repo.h"

// Indicates to hybrid graphics systems to prefer the discrete part by default.
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
// extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 618; }
// extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = reinterpret_cast<const char*>(u8".\\D3D12\\"); }

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace Monad::Dev;
using namespace Monad::Exceptions;
using namespace Monad::Kernel;
using namespace Monad::Renderer;
using namespace Monad::Renderer::HDR;
using namespace Monad::Time;
using namespace Monad::Tools;
using namespace Monad::Wnd;

uint32_t DXSample::m_width{ 1920 }, DXSample::m_height{ 1080 };
extern uint32_t g_shaderResourceViewCount = 0;

namespace
{
	constexpr uint32_t
		FRONT_BUFFER = 1,
		BACK_BUFFER = 1,
		BACK_AND_FRONT_BUFFERS = BACK_BUFFER + FRONT_BUFFER;

	inline uint32_t CountCBuffers(
		CB::MapConstantBufferManager::InitializerListConstantBufferManager cbTypeGenerics
	) noexcept
	{
		return std::ranges::fold_left(
			cbTypeGenerics,
			0u,
			[](
				const uint32_t prevValue,
				const CB::MapConstantBufferManager::value_type& currentConstantBufferManager) noexcept
			{
				return prevValue + currentConstantBufferManager.second.GetCount();
			});
	}

	inline DXGI_FORMAT NoSRGBRenderTarget(
		DXGI_FORMAT fmt
	)
	{
		switch (fmt)
		{
			[[likely]]
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
			[[unlikely]]
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
			[[unlikely]]
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
			[[unlikely]]
		default:                                return fmt;
		}
	}
}

DXSampleGeneric::DXSampleGeneric() noexcept
{
	CB::ZeroGroupOffset();
}

DXSample::DXSample(
	InputLayout::MapInputLayouts inputLayouts,
	const Counts counts,
	const uint32_t sampleCount,
	MapFX::InitializerListFXes fxCollection,
	InitializerListTechniques techniques,
	CB::MapConstantBufferManager::InitializerListConstantBufferManager constantBufferManager
) :
	c_me(
		this,
		g_dxSample
	),
	c_inputLayouts(inputLayouts),
	m_techniques(techniques),
	c_sampleCount(sampleCount),
	m_cbSrCurrentID(
		CountCBuffers(constantBufferManager)
	),
	m_rtCurrentID(BACK_AND_FRONT_BUFFERS),
	m_dsCurrentID(0),
	c_countOfCBSR(
		m_cbSrCurrentID.load()
		+ counts.c_bareCbvSrvCount
		+ counts.c_foundSrvFilesCount
	),
	m_constantBufferManager(constantBufferManager),
	m_fxCollection(fxCollection),
	m_currentSwapChainColorSpace(
		g_HDRConfig[_8].m_currentSwapChainColorSpace
	),
	m_currentSwapChainBitDepth(_8)
{
	assert(2 == sampleCount || 4 == sampleCount || 8 == sampleCount);
	CheckTearingSupport();
	g_frameIndex = 0;
}

DXSample::~DXSample()
{
#if defined _DEBUG	
	wstringstream strm;
	strm << L"Constant Buffers";
	uint32_t count = 0;
	for (const auto& ctrl : m_constantBufferManager)
	{
		strm << L"\n\t\t{ \""s
			<< Monad::Tools::UTF8ToUnicode(ctrl.first)
			<< L"\"_constantBuffer, { "s
			<< ToPascalOrCamelCase(Monad::Tools::UTF8ToUnicode(ctrl.first))
			<< L", "s
			<< to_wstring(ctrl.second.m_resCounter.m_counterOfUsages)
			<< L" } },"s;
		count += ctrl.second.m_resCounter.m_counterOfUsages;
	}
	OutputDebugString(L"\nlist: ");
	OutputDebugString(strm.str().c_str());
	OutputDebugString(L"\ncount: ");
	OutputDebugString(to_wstring(count).c_str());
	OutputDebugString(L"\nConstant Buffers-End\n");
#endif
	WaitForGpu();
}

void DXSample::OnFrameRenderAll(
	const STAGE_MODES mode
)
{
	g_stageOfRendering = mode;
	/*DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixRotationY(-1));
	OnFrameRender(mat);*/
	XMFLOAT4X4 matrixIDentity;
	XMStoreFloat4x4(
		&matrixIDentity,
		XMMatrixIdentity()
	);
	OnFrameRender(matrixIDentity);
}

void DXSample::CheckTearingSupport() noexcept
{
#ifndef PIXSUPPORT
	ComPtr<IDXGIFactory6> factory;
	auto hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
	auto allowTearing = FALSE;
	if (SUCCEEDED(hr))
		hr = factory->CheckFeatureSupport(
			DXGI_FEATURE_PRESENT_ALLOW_TEARING,
			&allowTearing,
			sizeof(allowTearing)
		);

	m_tearingSupport = SUCCEEDED(hr) && allowTearing;
#else
	m_tearingSupport = TRUE;
#endif
}

#if defined _DEBUG
const SystemLibraryDXGI g_DXGILibary;
void DXSample::ReportLiveObjects()
{
	ComPtr<IDXGIDebug> pDxgiDebug;

	g_DXGILibary.c_fnDXGIGetDebugInterface(
		__uuidof(IDXGIDebug),
		(void**)&pDxgiDebug
	);
	pDxgiDebug->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_ALL
	);
}

void DXSample::ReportPresent()
{
	g_DXGILibary.c_fnDXGIGetDebugInterface(
		__uuidof(IDXGIInfoQueue),
		(void**)&m_dxgiInfoQueue
	);
}

wstring DXSample::LogInfoQue()
{
	struct VectorMessage : VectorNoCtor<uint8_t>
	{
		D3D12_MESSAGE* GetD3D() noexcept
		{
			return reinterpret_cast<D3D12_MESSAGE*>(this->data());
		}

		DXGI_INFO_QUEUE_MESSAGE* GetDXGI() noexcept
		{
			return reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(this->data());
		}
	};

	wstring res;
	{
		const uint64_t messageCount = m_d3dInfoQueue->GetNumStoredMessages();
		for (uint64_t i = 0; i < messageCount; i++)
		{
			size_t messageSize = 0;
			m_d3dInfoQueue->GetMessage(
				i,
				nullptr,
				&messageSize
			); //get the size of the message

			VectorMessage message;
			message.resize(messageSize);

			if (!message.empty())
				try
				{
					if (SUCCEEDED(m_d3dInfoQueue->GetMessage(
						i,
						message.GetD3D(),
						&messageSize)))
						res += UTF8ToUnicode(message.GetD3D()->pDescription);
				}
				MONAD_CATCH;
		}
		m_d3dInfoQueue->ClearStoredMessages();
	}
	{
		const uint64_t messageCount = m_dxgiInfoQueue->GetNumStoredMessages(
			DXGI_DEBUG_ALL
		);
		for (uint64_t i = 0; i < messageCount; i++)
		{
			size_t messageSize = 0;
			m_dxgiInfoQueue->GetMessage(
				DXGI_DEBUG_ALL,
				i,
				nullptr,
				&messageSize
			);

			VectorMessage message;
			message.resize(messageSize);

			if (!message.empty())
				try
				{
					if (SUCCEEDED(m_dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, message.GetDXGI(), &messageSize)))
						res += UTF8ToUnicode(message.GetDXGI()->pDescription);
				}
				MONAD_CATCH;
		}
		m_dxgiInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
	}
	return res;
}
#endif

void DXSample::OnD3DCreateAllocators()
{
	for (auto& frame : m_frames)
	{
		THROW_EXC_D3D(m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&frame.m_commandAllocator)),
			L"Create Command Allocator"
		);
		NAME_D3D12_OBJECT(frame.m_commandAllocator);
	}
}

void DXSample::OnD3DCreateCommandList()
{
	THROW_EXC_D3D(
		m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_frames->m_commandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(&m_commandList)),
		L"Create Command List"
	);
	NAME_D3D12_OBJECT(m_commandList);

	// Close the command lists.
	THROW_EXC_D3D(m_commandList.Get()->Close(), L"Command List Close");
	THROW_EXC_D3D(m_device->CreateFence(
		m_frames->m_fenceValue,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_commandList.m_fence)), L"Create Fence");
	NAME_D3D12_OBJECT(m_commandList.m_fence);
}

void DXSample::OnD3DReset()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	THROW_EXC_D3D(m_frames->m_commandAllocator->Reset(), L"Command Allocator Reset");
	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	THROW_EXC_D3D(m_commandList.Get()->Reset(
		m_frames->m_commandAllocator.Get(),
		nullptr),
		L"Command List Reset"
	);

	ID3D12DescriptorHeap* const ppHeaps[]{ *m_cbvSrvHeap };
	m_commandList.Get()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void DXSample::MoveToNextFrame()
{
	// Schedule a Signal command in the queue.
	const auto currentFenceValue = m_frames->m_fenceValue;
	THROW_EXC_D3D(
		m_commandQueue->Signal(
			m_commandList.m_fence.Get(),
			currentFenceValue),
		L"Command Queue Current Fence Nxt"
	);

	// Update the frame index.
	g_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, WaitForEndOfStreams until it is ready.
	if (m_commandList.m_fence->GetCompletedValue() < m_frames->m_fenceValue)
		m_commandList.WaitForCompletionEvent(m_frames->m_fenceValue);

	// Set the fence value for the next frame.
	m_frames->m_fenceValue = currentFenceValue + 1;
}

void DXSample::CreateOneRootSignature(
#if defined _DEBUG
	const wstring& name,
#endif
	const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc,
	PtrRootSignature& rootSignature
)
{
	ComPtr<ID3DBlob> signature;
	THROW_EXC_D3D(D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_1,
		&signature,
		nullptr),
		L"Serialize Versioned Root Signature"
	);
	THROW_EXC_D3D(m_device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)),
		L"Create Root Signature"
	);
#if defined _DEBUG
	SetName(
		rootSignature.Get(),
		name.c_str());
#endif
}

void DXSample::WaitForGpu() noexcept
{
	if (IsValid() && m_commandQueue)
		try
		{
			auto& fenceValue = m_frames->m_fenceValue;
			// Schedule a Signal command in the queue.
			THROW_EXC_D3D(m_commandQueue->Signal(
				m_commandList.m_fence.Get(),
				fenceValue),
				L"Command Queue Signal"
			);

			// Wait until the fence has been processed.
			m_commandList.WaitForCompletionEvent(fenceValue);

			// Increment the fence value for the current frame.
			fenceValue++;
		}
		MONAD_CATCH;
}

void DXSample::LoadSizeDependentResources()
{
	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Create a RTV for each frame.
		for (uint32_t n = 0; n < FRAME_COUNT; ++n)
		{
			THROW_EXC_D3D(m_swapChain->GetBuffer(
				n,
				IID_PPV_ARGS(&m_frames[n].m_renderTargetRes)),
				L"Get RTV"
			);
			m_device->CreateRenderTargetView(
				m_frames[n].m_renderTargetRes.Get(),
				&m_renderTargetViewDesc.m_desc,
				rtvHandle
			);
			rtvHandle.Offset(
				1,
				m_rtvHeap->GetDescriptorSize()
			);

			NAME_D3D12_OBJECT(m_frames[n].m_renderTargetRes);
		}
	}

#pragma region MSAA
	m_msaaTargets.CreateRenderTargetDepthStencil(
#if defined _DEBUG
		L"MSAA",
#endif
		{
			static_cast<int32_t>(m_rtCurrentID++),
			m_renderTargetMSAAViewDesc,
			std::nullopt,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE
		},
		{
			static_cast<int32_t>(m_dsCurrentID++),
			DEPTH_STENCIL_VIEW_DESC_MSAA
		},
		m_width,
		m_height,
		c_sampleCount
	);
#pragma endregion MSAA
}

void DXSample::LoadAssets()
{
	g_timer.Start();

	// Single-use command allocator and command list for creating resources.
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	GraphicsCommandList1 commandList;

	THROW_EXC_D3D(m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator)),
		L"Create Command Allocator");
	NAME_D3D12_OBJECT(commandAllocator);
	THROW_EXC_D3D(
		m_device->CreateCommandList(
			0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&commandList)),
		L"List");
	NAME_D3D12_OBJECT(commandList);

	// Create the command lists.
	OnD3DCreateCommandList();
	FireEvent(EVENT_IDS::D3D_CREATE);
	Monad::Repositories::g_repositoryGeneric->AddGhostTexture();
	Repo();

	LoadSizeDependentResources();
	// LoadSceneResolutionDependentResources();

	// Close the resource creation command list and execute it to begin the vertex buffer copy into
	// the default heap.
	THROW_EXC_D3D(
		commandList.Get()->Close(),
		L"Tempoeary Command List Close"
	);
	ID3D12CommandList* ppCommandLists[]{ commandList.Get() };
	m_commandQueue->ExecuteCommandLists(
		_countof(ppCommandLists),
		ppCommandLists
	);

	// Create synchronization objects and WaitForEndOfStreams until assets have been uploaded to the GPU.
	m_frames->m_fenceValue++;

	// Wait for the command list to execute before continuing.
	WaitForGpu();
}

void DXSample::LoadPipeline()
{
#if defined _DEBUG
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	if (ComPtr<ID3D12Debug3> debugController; SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
		debugController->SetEnableSynchronizedCommandQueueValidation(TRUE);
		debugController->SetGPUBasedValidationFlags(
			D3D12_GPU_BASED_VALIDATION_FLAGS_NONE
		);
	}
#endif
	assert(m_height);
	const auto
		dx = static_cast<float>(m_width),
		dy = static_cast<float>(m_height);
	XMStoreFloat4x4(
		&m_matrices.m_matrixProjection,
		XMMatrixPerspectiveFovLH(
			XM_PIDIV4,
			dx / dy,
			1.0f,
			20.0f)
	);
	m_matrices.m_monitorProps.ProcessAspectRatio(
		dx,
		dy
	);

	THROW_EXC_D3D(CreateDXGIFactory2(
		DXGI_FACTORY_FLAGS,
		IID_PPV_ARGS(&m_dxgiFactory)),
		L"Create DXGI Factory2"
	);

	ComPtr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter(m_dxgiFactory.Get(), &hardwareAdapter);

	THROW_EXC_IFFAILED(CapsFailed, D3D12CreateDevice(
		hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_device)
	),
		L"3D Create D3D12 Device"
	);
	NAME_D3D12_OBJECT(m_device);
#if defined _DEBUG
	{
		THROW_EXC_D3D(m_device.As(&m_d3dInfoQueue), L"NewInfo");
	}
#endif

	// Describe and create the command queue.
	THROW_EXC_D3D(m_device->CreateCommandQueue(
		&COMMAND_QUEUE_DESC,
		IID_PPV_ARGS(&m_commandQueue)), L"Create Command Queue"
	);
	NAME_D3D12_OBJECT(m_commandQueue);

	//CheckDisplayHDRSupport();
	//m_enableST2084 = m_hdrSupport;
	m_renderTargetViewDesc = {
		GetCurrentHDRConfig().m_hdrFormat,
		FLOAT_4_ONES,
		D3D12_RTV_DIMENSION_TEXTURE2D
	};
	m_renderTargetMSAAViewDesc = {
		GetCurrentHDRConfig().m_hdrFormat,
		FLOAT_4_ONES,
		D3D12_RTV_DIMENSION_TEXTURE2DMS
	};

	// Describe and create the swap chain.
	// The resolution of the swap chain buffers will match the resolution of the window, enabling the
	// app to enter iFlip when in fullscreen mode. We will also keep a separate buffer that is not part
	// of the swap chain as an intermediate render target, whose resolution will control the rendering
	// resolution of the scene.
	const DXGI_SWAP_CHAIN_DESC1 swapChainDesc{
	.Width = m_width,
	.Height = m_height,
	.Format = NoSRGBRenderTarget(GetCurrentHDRConfig().m_hdrFormat),
	.SampleDesc {.Count = 1, .Quality = 0 },
	.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	.BufferCount = FRAME_COUNT,
	.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
	.Flags = m_tearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
	};

	/*constexpr DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreen
	{
		{ 0, 0 },
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		DXGI_MODE_SCALING_UNSPECIFIED,
		FALSE
	};*/

	ComPtr<IDXGISwapChain1> swapChain;
	THROW_EXC_D3D(m_dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		Monad::Globals::g_mainWindow,
		&swapChainDesc,
		nullptr, //m_tearingSupport ? nullptr : &swapChainFullscreen,
		nullptr,
		&swapChain
	), L"Create Swap Chain For Hwnd"
	);

	if (m_tearingSupport)
		// When tearing support is enabled we will handle ALT+Enter key presses in the
		// window message loop rather than let DXGI handle it by calling SetFullscreenState.
		m_dxgiFactory->MakeWindowAssociation(Monad::Globals::g_mainWindow, DXGI_MWA_NO_ALT_ENTER);

	THROW_EXC_D3D(
		swapChain.As(&m_swapChain),
		L"As Swap Chain"
	);
	// Check display Hdr support and initialize ST.2084 support to match the display's support.
	EnsureSwapChainColorSpace(_10, m_enableST2084);
	if (DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 == m_currentSwapChainColorSpace)
		SetHDRMetaData();

	g_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target (RTV) descriptor heap.
		const D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			m_countOfRT + BACK_AND_FRONT_BUFFERS
		};
		m_rtvHeap = std::make_unique<HeapCombine>(
			m_device,
			rtvHeapDesc
		);
		// Describe and create a depth stencil view (DSV) descriptor heap.
		const D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			m_countOfDS
		};
		m_dsvHeap = std::make_unique<HeapCombine>(
			m_device,
			dsvHeapDesc
		);
		// Describe and create a constant buffer view (SRV) and shader resource view (SRV) descriptor heap.
		const D3D12_DESCRIPTOR_HEAP_DESC cbvSrvHeapDesc{
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			c_countOfCBSR
			+ m_countOfRT
			+ m_countOfDS,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		};
		m_cbvSrvHeap = std::make_unique<HeapCombine>(
			m_device,
			cbvSrvHeapDesc
		);
	}

	// Create command allocators for each frame.
	OnD3DCreateAllocators();
}

bool DXSample::OnBoth()
{
	if (!m_device)
		return false;

	g_timer.GetTimeValue(g_fTime);
	OnUpdate();
	OnRender();

	return true;
}

void DXSample::GetHardwareAdapter(
	IDXGIFactory6* factory,
	IDXGIAdapter1** pAdapter,
	bool requestHighPerformanceAdapter
)
{
	Monad::Globals::g_adapterDesc.clear();

	*pAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

	ComPtr<IDXGIFactory6> factory6;
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
				Monad::Globals::g_adapterDesc = desc.Description;
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
				Monad::Globals::g_adapterDesc = desc.Description;
				break;
			}
		}
	}
	*pAdapter = adapter.Detach();
}

const HDRProfile& DXSample::GetCurrentHDRConfig() const noexcept
{
	return g_HDRConfig[m_currentSwapChainBitDepth];
}

void DXSample::EnsureSwapChainColorSpace(
	SWAP_CHAIN_BIT_DEPTH swapChainBitDepth,
	bool enableST2084
)
{
	DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
	switch (swapChainBitDepth)
	{
		/*case _8:
			break;*/

	case _10:
		colorSpace =
			enableST2084
			? DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020
			: DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
		break;

		/*case _16:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
			break;*/
	}

	//if (m_currentSwapChainColorSpace != colorSpace)
	{
		UINT colorSpaceSupport = 0;
		if (SUCCEEDED(m_swapChain->CheckColorSpaceSupport(
			colorSpace,
			&colorSpaceSupport))
			&& ((colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) == DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
		{
			THROW_EXC_D3D(m_swapChain->SetColorSpace1(colorSpace), L"ClrSpc");
			m_currentSwapChainColorSpace = colorSpace;
		}
	}
}

void DXSample::SetHDRMetaData(float MaxOutputNits /*=1000.0f*/, float MinOutputNits /*=0.001f*/, float MaxCLL /*=2000.0f*/, float MaxFALL /*=500.0f*/)
{
	if (!m_swapChain)
		return;

	// Clean the hdr metadata if the display doesn't support Hdr
	if (!m_hdrSupport)
	{
		THROW_EXC_D3D(m_swapChain->SetHDRMetaData(
			DXGI_HDR_METADATA_TYPE_NONE,
			0,
			nullptr), L"Set HDR Meta Data");
		return;
	}

	constexpr DisplayChromaticities Chroma =
	{
		0.70800f,
		0.29200f,
		0.17000f,
		0.79700f,
		0.13100f,
		0.04600f,
		0.31270f,
		0.32900f
	}; // Display Gamut Rec2020

	DXGI_HDR_METADATA_HDR10 HDR10MetaData{
	.RedPrimary{
			static_cast<uint16_t>(Chroma.RedX * 50000.0f),
			static_cast<uint16_t>(Chroma.RedY * 50000.0f)
	},
	.GreenPrimary{
			static_cast<uint16_t>(Chroma.GreenX * 50000.0f),
			static_cast<uint16_t>(Chroma.GreenY * 50000.0f)
	},
	.BluePrimary{
			static_cast<uint16_t>(Chroma.BlueX * 50000.0f),
			static_cast<uint16_t>(Chroma.BlueY * 50000.0f)
	},
	.WhitePoint{ static_cast<uint16_t>(
		Chroma.WhiteX * 50000.0f),
		static_cast<uint16_t>(Chroma.WhiteY * 50000.0f)
	},
	.MaxMasteringLuminance = static_cast<uint32_t>(MaxOutputNits * 10000.0f),
	.MinMasteringLuminance = static_cast<uint32_t>(MinOutputNits * 10000.0f),
	.MaxContentLightLevel = static_cast<uint16_t>(MaxCLL),
	.MaxFrameAverageLightLevel = static_cast<uint16_t>(MaxFALL) };
	THROW_EXC_D3D(m_swapChain->SetHDRMetaData(
		DXGI_HDR_METADATA_TYPE_HDR10,
		sizeof(DXGI_HDR_METADATA_HDR10),
		&HDR10MetaData),
		L"Set HDR Meta Data #2"
	);
}

void DXSample::OnBackBuffer()
{
	auto backBuffer = m_frames->m_renderTargetRes.Get();
	// Resolve the MSAA render target.
	const auto scopedTransition = m_commandList.ScopedTransition({ {
		backBuffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RESOLVE_DEST } }
		);

	m_commandList.ResolveSubresource(
		backBuffer,
		m_msaaTargets
	);
}

DXSample* g_dxSample = nullptr;
