// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Tex.h"
#include "Kernel/Del.h"

namespace Monad::Renderer
{
	interface RenderTargetDepthStencilGeneric abstract
	{
	protected:
		RenderTargetDepthStencilGeneric() = default;

	public:
		void reset() noexcept;
		interface PassIDs abstract
		{
			const int32_t c_typeID;
			const OptionalUInt m_texID;
			const D3D12_RESOURCE_STATES c_resourceState;
		};
		struct PassIDsRenderTarget : PassIDs
		{
			PassIDsRenderTarget(
				const int32_t renderTargetID,
				const FormatAndClearRenderTarget& viewDesc,
				const OptionalUInt texID = std::nullopt,
				const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_RESOLVE_SOURCE
			) noexcept;

			const FormatAndClearRenderTarget& c_viewDesc;
		};
		struct PassIDsDepthStencil : PassIDs
		{
			PassIDsDepthStencil(
				const int32_t depthStencilID,
				const FormatAndClearDepthStencil& viewDesc,
				const D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE
			) noexcept;

			const FormatAndClearDepthStencil& c_viewDesc;
		};

	protected:
		using OptionalPassIDsRenderTarget = std::optional<PassIDsRenderTarget>;
		using OptionalPassIDsDepthStencil = std::optional<PassIDsDepthStencil>;
		void CreateTargetDepthStencilGeneric(
#if defined _DEBUG
			std::wstring name,
#endif
			const OptionalPassIDsRenderTarget& renderTarget,
			const OptionalPassIDsDepthStencil& depthStencil,
			long dx,
			long dy,
			uint32_t sampleCount = 1
		);
		OPER_DEL_NO_DEF_CTOR(RenderTargetDepthStencilGeneric);

	public:
		void SetMe() noexcept;

		Texture& GetRtTexture() noexcept;
		ID3D12Resource* GetRtResource() noexcept;
		const D3D12_CLEAR_VALUE& GetRtClearValue() const noexcept;

		Texture& GetDsTexture() noexcept;
		ID3D12Resource* GetDsResource() noexcept;

	private:
		struct OneRes
		{
			inline OneRes(
				int32_t id,
				long dx,
				long dy,
				DXGI_FORMAT format,
				D3D12_RESOURCE_FLAGS flags,
				OptionalUInt resID,
				uint32_t sampleCount,
				D3D12_RESOURCE_STATES state,
				D3D12_CLEAR_VALUE clearValue
			);

			int32_t m_onHeapID;
			D3D12_CLEAR_VALUE c_clearValue;
			Texture m_texture;
		};
		std::optional<OneRes> c_renderTarget, c_depthStencil;
		D3D12_RECT m_scissorRect;
		D3D12_VIEWPORT m_viewPort;
	};

	struct RenderTarget final : RenderTargetDepthStencilGeneric
	{
		RenderTarget();
		void CreateRenderTarget(
#if defined _DEBUG
			std::wstring name,
#endif
			const PassIDsRenderTarget& renderTarget,
			long dx,
			long dy,
			uint32_t sampleCount = 1
		);

		DirectX::XMFLOAT4 m_clearColor;
	};

	struct DepthStencil : RenderTargetDepthStencilGeneric
	{
		DepthStencil();
		void CreateDepthStencil(
#if defined _DEBUG
			std::wstring name,
#endif
			const PassIDsDepthStencil& depthStencil,
			long dx,
			long dy,
			uint32_t sampleCount = 1
		);
	};

	struct RenderTargetDepthStencil : RenderTargetDepthStencilGeneric
	{
		RenderTargetDepthStencil();
		void CreateRenderTargetDepthStencil(
#if defined _DEBUG
			std::wstring name,
#endif
			const PassIDsRenderTarget& renderTarget,
			const PassIDsDepthStencil& depthStencil,
			long dx,
			long dy,
			uint32_t sampleCount = 1
		);
	};
}