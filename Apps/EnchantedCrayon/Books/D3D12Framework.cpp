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
#include "D3D12Framework.h"
// Monad
#include "__App.h"
#include "AppSingleton.h"
#include "GUI/__MonadGUI.h"
#include "Files/FileManager.h"
#include "Input/Cursor.h"
#include "Printer/Sketch.h"
#include "System/Parameter.h"
#include "Time/__MonadTime.h"
#include "Time/Timer.h"
#include "Win32Application.h"
#include "Renderer/__MonadRenderer.h"

using namespace DirectX;
using namespace std;
using Microsoft::WRL::ComPtr;
using namespace Monad::App;
using namespace Monad::Files;
using namespace Monad::GUI;
using namespace Monad::Input;
using namespace Monad::Kernel;
using namespace Monad::Pages;
using namespace Monad::Print;
using namespace Monad::Renderer;
using namespace Monad::System;
using namespace Monad::Threads;
using namespace Monad::Time;
using namespace Monad::Tools;

SingleCall g_setArtStubs(true);

D3D12Core::D3D12Core() :
	DXSample{
		{
			InputLayout::FontEndEmoji()
			+ InputLayout::Basics()
			+ InputLayout::Image()
		},
		{
			.c_bareCbvSrvCount = CBV_SRV_COUNT,
			.c_foundSrvFilesCount = g_shaderResourceViewCount
		},
	min(4u, 2u << g_applied.m_quality),
	{
		{
			"blank_tex"_fx,
			make_shared<ShaderConfig>(
				"blank_tex"_vertexShader,
				"blank_tex"_pixelShader,
				false
			)
		},
		{
			"color_button"_fx,
			make_shared<ShaderConfig>(
				"color_button"_vertexShader,
				"color_button"_pixelShader,
				false
			)
		},
		{
			"crayon"_fx,
			make_shared<ShaderConfig>(
				"crayon"_vertexShader,
				"crayon"_pixelShader,
				false
			)
		},
		{
			"emoji"_fx,
			make_shared<ShaderConfig>(
				"emoji"_vertexShader,
				"emoji"_pixelShader,
				true
			)
		},
		{
			"flag"_fx,
			make_shared<ShaderConfig>(
				"iconic"_vertexShader,
				"flag"_pixelShader,
				false
			)
		},
		{
			"font"_fx,
			make_shared<ShaderConfig>(
				"font"_vertexShader,
				"font"_pixelShader,
				true
			)
		},
		{
			"iconic"_fx,
			make_shared<ShaderConfig>(
				"iconic"_vertexShader,
				"iconic"_pixelShader,
				false
			)
		},
		{
			"logo"_fx,
			make_shared<ShaderConfig>(
				"raw_tex"_vertexShader,
				"logo"_pixelShader,
				false
			)
		},
		{
			"metallic_border"_fx,
			make_shared<ShaderConfig>(
				"metal_border"_vertexShader,
				"metal"_pixelShader,
				false
			)
		},
		{
			"metallic_hr"_fx,
			make_shared<ShaderConfig>(
				"metal_hr"_vertexShader,
				"metal"_pixelShader,
				false
			)
		},
		{
			"paper"_fx,
			make_shared<ShaderConfig>(
				"paper"_vertexShader,
				"paper"_pixelShader,
				false
			)
		},
		{
			"raw_tex"_fx,
			make_shared<ShaderConfig>(
				"raw_tex"_vertexShader,
				"raw_tex"_pixelShader,
				false
			)
		},
		{
			"wallpaper"_fx,
			make_shared<ShaderConfig>(
				"wallpaper"_vertexShader,
				"wood_tex"_pixelShader,
				false
			)
		},
		{
			"shadow_border"_fx,
			make_shared<ShaderConfig>(
				"shadow_border"_vertexShader,
				"shadow"_pixelShader,
				false
			)
		},
		{
			"shadow_emoji"_fx,
			make_shared<ShaderConfig>(
				"shadow_emoji"_vertexShader,
				"shadow_emoji"_pixelShader,
				false
			)
		},
		{
			"shadow_font"_fx,
			make_shared<ShaderConfig>(
				"shadow_font"_vertexShader,
				"shadow_font"_pixelShader,
				false
			)
		},
		{
			"shadow_hr"_fx,
			make_shared<ShaderConfig>(
				"shadow_hr"_vertexShader,
				"shadow"_pixelShader,
				false
			)
		},
		{
			"shadow"_fx,
			make_shared<ShaderConfig>(
				"shadow"_vertexShader,
				"shadow"_pixelShader,
				false
			)
		}
	},
	{
		{
			"blank_tex"_technique,
			"blank_tex"_fx
		},
		{
			"color_button"_technique,
			"color_button"_fx
		},
		{
			"crayon"_technique,
			{ "crayon"_fx, "shadow"_fx }
		},
		{
			"emoji"_technique,
			{ "emoji"_fx, "shadow_emoji"_fx }
		},
		{
			"flag"_technique,
			"flag"_fx },
		{
			"font"_technique,
			{ "font"_fx, "shadow_font"_fx }
		},
		{
			"iconic"_technique,
			"iconic"_fx
		},
		{
			"logo"_technique,
			"logo"_fx
		},
		{
			"metallic_border"_technique,
			{ "metallic_border"_fx, "shadow_border"_fx }
		},
		{
			"metallic_hr"_technique,
			{ "metallic_hr"_fx, "shadow_hr"_fx }
		},
		{
			"paper"_technique,
			{ "paper"_fx, "shadow"_fx }
		},
		{
			"raw_tex"_technique,
			"raw_tex"_fx
		},
		{
			"wallpaper"_technique,
			"wallpaper"_fx
		}
	},
	{
		{ "forecolor"_constantBuffer, { sizeof DirectX::XMFLOAT3, 76 } },
		{ "hdr"_constantBuffer, { sizeof BOOL, 1u } },	
		{ "light_pos"_constantBuffer, { sizeof DirectX::XMFLOAT4, 1 } },
		{ "model"_constantBuffer, { sizeof CB::Model, 240 } },
		{ "pipe_color"_constantBuffer, { sizeof CB::PipeColor, 25 } },
		{ "shadow"_constantBuffer, { sizeof DirectX::XMFLOAT4X4, 240 } }
	} }
{
	LoadPipeline();
	LoadAssets();
}

D3D12Core::~D3D12Core()
{
	g_repository->WriteCustomerData();
}

/*void D3D12Framework::ResizeWindow() noexcept
{
	const RECT rect = ResizeWindowGeneric();

	OnSizeChanged(rect.right, rect.bottom, IsIconic(Win32ApplicationGeneric::GetHwnd()));
}*/

void D3D12Core::OnUpdate()
{
	switch (g_fileManagerState)
	{
		[[unlikely]]
	case STOP_BOOT:
	{
		g_singleton->InsertTaskSearcher2(LOAD_BOOT_DX);

		if (static SingleCall initializedAudioOnce; !initializedAudioOnce.test_and_set())
			Monad::Audio::g_persistentAudio->SetMuted();
		break;
	}
	[[unlikely]]
	case STOP_BOOT_DX:
	{
		g_singleton->m_cursorControl.SetNewCurrentCursor(nullptr);
		SetMyCursor();
		g_singleton->m_d3dSingleton->Initialize();

		uint32_t shadowWidth, shadowHeight;
		switch (g_applied.m_quality)
		{
		case 0:
			shadowWidth = 1280;
			shadowHeight = 720;
			break;
		case 1:
			shadowWidth = GetOptimalMapSize({
					{3840U, 4096U},
					{1200U, 2048U}
				},
				m_width,
				1024U	
			);
			shadowHeight = GetOptimalMapSize({
					{1920U, 4096U},
					{1080U, 2048U}
				},
				m_height,
				1024U);
			break;
		default:
			shadowWidth = GetOptimalMapSize({
					{15360U, 16384U},
					{7680U, 8192U},
					{3840U, 4096U},
					{1200U, 2048U}
				},
				m_width,
				1024U
			);
			shadowHeight = GetOptimalMapSize({
					{8640U, 16384U},
					{4320U, 8192U},
					{1920U, 4096U},
					{1080U, 2048U}
				},
				m_height,
				1024U);
			break;
		}

		m_shadows.CreateDepthStencil(
#if defined _DEBUG
			L"Shadow",
#endif
			{
				static_cast<int32_t>(m_dsCurrentID++),
				DEPTH_STENCIL_VIEW_DESC,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			},
			shadowWidth,
			shadowHeight
			);
		g_singleton->InsertTaskSearcher2(LOAD_ALL);
		ResetCurrentPage();
		break;
	}
	case STOP_ALL:
	{
		if (!g_profileOnce.test_and_set() && PathFileExists(g_singleton->c_profilePath.c_str()) != FALSE)
		{
			g_singleton->InsertTaskSearcher2(LOAD_PROFILE);
			break;
		}
	}
	[[fallthrough]];
	case STOP_PROFILE:
	{
		assert(!g_repository->m_memArts.empty());
		g_fileManagerState = CHECK_ALL;
		struct TaskMergeAll final : ThreadPool::ITask
		{
			void Run() final
			{
				g_repository->ImportArts();
			}
		};
		try
		{
			ComPtr<TaskMergeAll> taskMergeAll;
			taskMergeAll.Attach(new TaskMergeAll);
			g_singleton->InsertTask(taskMergeAll.Get());
			taskMergeAll.Detach();
			break;
		}
		MONAD_CATCH;
		break;
	}
	case MERGE_ALL:
		g_fileManagerState = FILE_MAN_STATE_READY;
		g_singleton->CreateFull();
		g_setArtStubs.clear();
		SelectPage(ResetPageBackup());
		break;
	}

	FireEventOnCopy(EVENT_IDS::FRAME_MOVE_STUB);

	if (!g_active)
		if (POINT curPos; GetCursorPos(&curPos))
			SetCursorPosGeneric(curPos.x, curPos.y);
}

void D3D12Core::OnRender()
{
	// Record all the commands we need to render the scene into the command lists.
/*#if defined MONAD_PIX
	PIXBeginEvent(m_commandQueue.Get(), 0, L"Render");
#endif*/
	PopulateCommandLists();
	/*#if defined MONAD_PIX
		PIXEndEvent(m_commandQueue.Get());
	#endif*/

	// Execute the command lists.
	ID3D12CommandList* ppCommandLists[]{ m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(
		_countof(ppCommandLists),
		ppCommandLists);

	if (nullptr != g_shaderSketchGeneric)
		g_shaderSketchGeneric->OnPrint();

	// When using sync interval 0, it is recommended to always pass the tearing
	// flag when it is supported, even when presenting in windowed mode.
	// However, this flag cannot be used if the app is in fullscreen mode as a
	// result of calling SetFullscreenState.

	// Present the frame.
	switch (const HRESULT hr = m_swapChain->Present(
		g_singleton->m_config.m_vSync,
		m_tearingSupport && !g_singleton->m_config.m_vSync
		? DXGI_PRESENT_ALLOW_TEARING
		: 0
	); hr)
	{
	case DXGI_ERROR_DEVICE_REMOVED:
#if defined _DEBUG
	{
		char buff[64] = "";
		sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
			(hr == DXGI_ERROR_DEVICE_REMOVED)
			? m_device->GetDeviceRemovedReason() : hr);
		OutputDebugStringA(buff);
	}
#endif
	case DXGI_ERROR_DEVICE_RESET:
		g_singleton->ResetDevice();
		break;
	default:
		THROW_EXC_D3D(hr, L"Present");
		NewFrame();
	}
}

void D3D12Core::NewFrame()
{
	MoveToNextFrame();
	if (nullptr != g_shaderSketchGeneric)
		g_shaderSketchGeneric->ScreenshotRT();
}

/*D3D12Core::~D3D12Core()
{
	g_hourGlassBlockingRefs.WaitForEndOfStreams();
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
}*/

void D3D12Core::PopulateCommandLists()
{
	OnD3DReset();
	FireEvent(EVENT_IDS::D3D_RENDER);
	m_commandList.Get()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	// Populate m_sceneCommandList to render scene to intermediate render target.
#pragma region Shadow
	if (STOP_BOOT_DX < g_fileManagerState)
	{
		DequeuePage();
		const auto scopedTransition = m_commandList.ScopedTransition({ {
			m_shadows.GetDsResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE } }
			);
		m_shadows.SetMe();
		OnFrameRenderAll(STAGE_MODES::SHADOW);
	}
	else 
		CenterCursor();
#pragma endregion

#pragma region Temporary monads
	OnFrameRenderAll(STAGE_MODES::CUSTOM);
	if (!g_setArtStubs.test_and_set())
		g_combineFullRequirements->SetArt(
			g_repository->m_currentArtProfile.m_currentArt
		);
#pragma endregion

#pragma region World			
	{
		const auto scopedTransition = m_commandList.ScopedTransition({ {
			m_msaaTargets.GetRtResource(),
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET } }
			);
		m_msaaTargets.SetMe();
		OnFrameRenderAll(STAGE_MODES::WORLD);
#pragma endregion

#pragma region Alpha blending
		OnFrameRenderAll(STAGE_MODES::ALPHA_BLENDING);
#pragma endregion
	}
	OnBackBuffer();
	m_commandList.Get()->Close();
}