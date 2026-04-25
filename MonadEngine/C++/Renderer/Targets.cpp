// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Targets.h"
// STD
#include <cstdint>
#include <string>
// Monad
#include "Core/DXSample.h"
#include "Core/DXSampleHelper.h"

namespace Monad::Renderer
{
#pragma region RenderTargetDepthStencilGeneric
	void RenderTargetDepthStencilGeneric::reset() noexcept
	{
		if (c_renderTarget)
			GetRtTexture().m_res.Reset();
		if (c_depthStencil)
			GetDsTexture().m_res.Reset();
	}
	void RenderTargetDepthStencilGeneric::CreateTargetDepthStencilGeneric(
#if defined _DEBUG
		std::wstring name,
#endif
		const OptionalPassIDsRenderTarget& renderTarget,
		const OptionalPassIDsDepthStencil& depthStencil,
		long dx,
		long dy,
		uint32_t sampleCount
	)
	{
		m_scissorRect = {
			0,
			0,
			dx,
			dy
		};
		m_viewPort = {
			0.0f,
			0.0f,
			static_cast<float>(dx),
			static_cast<float>(dy),
			D3D12_MIN_DEPTH,
			D3D12_MAX_DEPTH
		};
		if (renderTarget)
		{
			c_renderTarget.emplace(
				renderTarget->c_typeID,
				dx,
				dy,
				renderTarget->c_viewDesc.m_desc.Format,
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
				renderTarget->m_texID,
				sampleCount,
				renderTarget->c_resourceState,
				renderTarget->c_viewDesc.m_clearValue
			);
			if (nullptr != GetRtResource())
			{
				g_dxSample->m_device->CreateRenderTargetView(
					GetRtResource(),
					&renderTarget->c_viewDesc.m_desc,
					g_dxSample->m_rtvHeap->GetCPUDescriptorHandle(
						renderTarget->c_typeID)
				);
#if defined _DEBUG
				SetName(
					GetRtResource(),
					(name + L"-m_renderTarget").c_str()
				);
#endif
			}
		}
		if (depthStencil)
		{
			c_depthStencil.emplace(
				depthStencil->c_typeID,
				dx,
				dy,
				depthStencil->c_viewDesc.m_desc.Format,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
				depthStencil->m_texID,
				sampleCount,
				depthStencil->c_resourceState,
				depthStencil->c_viewDesc.m_clearValue
			);
			if (nullptr != GetDsResource())
			{
				g_dxSample->m_device->CreateDepthStencilView(
					GetDsResource(),
					&depthStencil->c_viewDesc.m_desc,
					g_dxSample->m_dsvHeap->GetCPUDescriptorHandle(
						depthStencil->c_typeID)
				);
#if defined _DEBUG
				SetName(
					GetDsResource(), (name + L"m_depthStencil").c_str()
				);
#endif
			}
		}
	}

	void RenderTargetDepthStencilGeneric::SetMe() noexcept
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{}, dsvHandle{};
		using PDESCR = CD3DX12_CPU_DESCRIPTOR_HANDLE*;
		PDESCR rtv, dsv;
		uint32_t countOfRT;
		if (c_renderTarget)
		{
			countOfRT = 1;
			rtvHandle = g_dxSample->m_rtvHeap->GetCPUDescriptorHandle(
				c_renderTarget->m_onHeapID
			);
			rtv = &rtvHandle;
		}
		else
		{
			countOfRT = 0;
			rtv = nullptr;
		}
		if (c_depthStencil)
		{
			dsvHandle = g_dxSample->m_dsvHeap->GetCPUDescriptorHandle(
				c_depthStencil->m_onHeapID
			);
			dsv = &dsvHandle;
		}
		else
			dsv = nullptr;

		g_dxSample->m_commandList.Get()->RSSetViewports(1, &m_viewPort);
		g_dxSample->m_commandList.Get()->RSSetScissorRects(1, &m_scissorRect);

		g_dxSample->m_commandList.Get()->OMSetRenderTargets(
			countOfRT,
			rtv,
			FALSE,
			dsv);

		if (c_renderTarget)
			g_dxSample->m_commandList.Get()->ClearRenderTargetView(
				rtvHandle,
				c_renderTarget->c_clearValue.Color,
				0,
				nullptr
			);
		if (c_depthStencil)
			g_dxSample->m_commandList.Get()->ClearDepthStencilView(
				dsvHandle,
				D3D12_CLEAR_FLAG_DEPTH,
				c_depthStencil->c_clearValue.DepthStencil.Depth,
				c_depthStencil->c_clearValue.DepthStencil.Stencil,
				0,
				nullptr
			);
	}

	Texture& RenderTargetDepthStencilGeneric::GetRtTexture() noexcept
	{
		assert(c_renderTarget);
		return c_renderTarget->m_texture;
	}

	ID3D12Resource* RenderTargetDepthStencilGeneric::GetRtResource() noexcept
	{
		return GetRtTexture().m_res.Get();
	}
	const D3D12_CLEAR_VALUE& RenderTargetDepthStencilGeneric::GetRtClearValue() const noexcept
	{
		return c_renderTarget->c_clearValue;
	}

	Texture& RenderTargetDepthStencilGeneric::GetDsTexture() noexcept
	{
		assert(c_depthStencil);
		return c_depthStencil->m_texture;
	}

	ID3D12Resource* RenderTargetDepthStencilGeneric::GetDsResource() noexcept
	{
		return GetDsTexture().m_res.Get();
	}
#pragma endregion

#pragma region RenderTargetDepthStencilGeneric::OneRes
	inline RenderTargetDepthStencilGeneric::OneRes::OneRes(
		int32_t id,
		long dx,
		long dy,
		DXGI_FORMAT
		format,
		D3D12_RESOURCE_FLAGS flags,
		OptionalUInt resID,
		uint32_t sampleCount,
		D3D12_RESOURCE_STATES state,
		D3D12_CLEAR_VALUE clearValue
	) :
		m_onHeapID{ id },
		c_clearValue{ clearValue },
		m_texture{
			resID,
			static_cast<uint32_t>(dx),
			static_cast<uint32_t>(dy),
			format,
			flags,
			sampleCount,
			state,
			&c_clearValue }
	{}
#pragma endregion

#pragma region RenderTargetDepthStencilGeneric::PassIDsRenderTarget
	RenderTargetDepthStencilGeneric::PassIDsRenderTarget::PassIDsRenderTarget(
		const int32_t renderTargetID,
		const FormatAndClearRenderTarget& viewDesc,
		const OptionalUInt texID,
		const D3D12_RESOURCE_STATES resourceState
	) noexcept :
		PassIDs{
		renderTargetID,
		texID,
		resourceState
	},
		c_viewDesc{ viewDesc }
	{}
#pragma endregion

#pragma region RenderTargetDepthStencilGeneric::PassIDsDepthStencil
	RenderTargetDepthStencilGeneric::PassIDsDepthStencil::PassIDsDepthStencil(
		const int32_t depthStencilID,
		const FormatAndClearDepthStencil& viewDesc,
		const D3D12_RESOURCE_STATES resourceState) noexcept :
		PassIDs{
			depthStencilID,
			std::nullopt,
			resourceState
		},
		c_viewDesc{ viewDesc }
	{}
#pragma endregion
	RenderTarget::RenderTarget()
	{
		g_dxSample->m_countOfRT++;
	}
	void RenderTarget::CreateRenderTarget(
#if defined _DEBUG
		std::wstring name,
#endif
		const PassIDsRenderTarget& renderTarget,
		long dx,
		long dy,
		uint32_t sampleCount
	)
	{
		CreateTargetDepthStencilGeneric(
#if defined _DEBUG
			name,
#endif
			renderTarget,
			std::nullopt,
			dx,
			dy,
			sampleCount
		);
	}

	DepthStencil::DepthStencil()
	{
		g_dxSample->m_countOfDS++;
	}
	void DepthStencil::CreateDepthStencil(
#if defined _DEBUG
		std::wstring name,
#endif
		const PassIDsDepthStencil& depthStencil,
		long dx,
		long dy,
		uint32_t sampleCount
	)
	{
		CreateTargetDepthStencilGeneric(
#if defined _DEBUG
			name,
#endif
			std::nullopt,
			depthStencil,
			dx,
			dy,
			sampleCount
		);
	}

	RenderTargetDepthStencil::RenderTargetDepthStencil()
	{
		g_dxSample->m_countOfRT++;
		g_dxSample->m_countOfDS++;
	}

	void RenderTargetDepthStencil::CreateRenderTargetDepthStencil(
#if defined _DEBUG
		std::wstring name,
#endif
		const PassIDsRenderTarget& renderTarget,
		const PassIDsDepthStencil& depthStencil,
		long dx,
		long dy,
		uint32_t sampleCount
	)
	{
		CreateTargetDepthStencilGeneric(
#if defined _DEBUG
			name,
#endif
			renderTarget,
			depthStencil,
			dx,
			dy,
			sampleCount
		);
	}
}