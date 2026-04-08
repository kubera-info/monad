// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Tex.h"
// Platform
#include <DirectXTex.h>
// Monad
#include "Core/DXSample.h"
#include "Core/DXSampleHelper.h"
#include "Stateless.h"
#include "Repository/Repo.h"
#include "Tools/Convert.h"

using namespace DirectX;

namespace Monad
{
	using namespace Exceptions;
	using namespace Kernel;

	namespace Renderer
	{
		namespace
		{
			inline D3D12_SRV_DIMENSION ToViewDimension(
				const D3D12_RESOURCE_DESC& resourceDesc
			) noexcept
			{
				switch (resourceDesc.Dimension)
				{
					[[likely]]
				case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
					return
						resourceDesc.SampleDesc.Count > 1
						? D3D12_SRV_DIMENSION_TEXTURE2DMS
						: D3D12_SRV_DIMENSION_TEXTURE2D;
					[[likely]]
				case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
					return D3D12_SRV_DIMENSION_TEXTURE1D;
					[[unlikely]]
				case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
					return D3D12_SRV_DIMENSION_TEXTURE3D;
					[[unlikely]]
				default:
					return D3D12_SRV_DIMENSION_UNKNOWN;
				}
			}

			inline D3D12_RESOURCE_DIMENSION ToResDimension(
				const TEX_DIMENSION textureDimension
			) noexcept
			{
				switch (textureDimension)
				{
					[[likely]]
				case TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D:
					return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					[[likely]]
				case TEX_DIMENSION::TEX_DIMENSION_TEXTURE1D:
					return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
					[[unlikely]]
				case TEX_DIMENSION::TEX_DIMENSION_TEXTURE3D:
					return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
					[[unlikely]]
				default:
					return D3D12_RESOURCE_DIMENSION_UNKNOWN;
				}
			}

			void CreateShaderResource(
				Texture& tex,
				const D3D12_RESOURCE_DESC& resourceDesc,
				D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_CLEAR_VALUE* clearValue = nullptr
			)
			{
				/*if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) > 0)
					ptrClearValue = &g_backBufferClearValue;
				else if ((resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) > 0)
					ptrClearValue = &g_depthClearValue;*/
				if (HRESULT hr; FAILED((hr = (g_dxSample->m_device->CreateCommittedResource(
					&HEAP_TYPE_DEFAULT,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					resourceState,
					clearValue,
					IID_PPV_ARGS(&tex.m_res))))))
				{
					thread_local Tools::OutputStringStream convStr;
					convStr << L"ResErr-A"
						<< L";Alignment=" << resourceDesc.Alignment
						<< L";Width=" << resourceDesc.Width
						<< L";Height=" << resourceDesc.Height
						<< L";Depth=" << resourceDesc.DepthOrArraySize
						<< L";MipLevels=" << resourceDesc.MipLevels
						<< L";Format=" << resourceDesc.Format
						<< L";SampleCount=" << resourceDesc.SampleDesc.Count
						<< L";SampleQuality=" << resourceDesc.SampleDesc.Quality
						<< L";Layout=" << resourceDesc.Layout
						<< L";Flags=" << resourceDesc.Flags;
					THROW_EXC_IFFAILED(
						Exceptions::CapsFailed,
						hr,
						convStr.str()
					);
				}
				NAME_D3D12_OBJECT(tex.m_res);
			}
		}

		Texture::Texture(
			const uint8_t* source,
			size_t sourceSize
		)
		{
			THROW_EXC_IFFALSE(ThreeDFailed, g_dxSample && g_dxSample->m_device, L"TexDDSg_dxSample");
			THROW_EXC_IFFALSE(ThreeDFailed, source && sourceSize, L"TexDDS");
			Stateless stateless;
			auto image = std::make_unique<ScratchImage>();
			PtrResource textureUploadHeap;
			HRESULT hr = LoadFromDDSMemory(
				source,
				sourceSize,
				DDS_FLAGS_NONE,
				nullptr,
				*image
			);

			if (FAILED(hr))
			{
				THROW_EXC_D3D(LoadFromWICMemory(
					source,
					sourceSize,
					WIC_FLAGS_FORCE_SRGB,
					nullptr,
					*image),
					L"WICTex"
				);
			}

			const auto& metadata = image->GetMetadata();

			const D3D12_RESOURCE_DESC textureDesc{
			.Dimension = ToResDimension(metadata.dimension),
			.Width = static_cast<UINT>(metadata.width),
			.Height = static_cast<UINT>(metadata.height),
			.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize),
			.MipLevels = static_cast<UINT16>(metadata.mipLevels),
			.Format = metadata.format,
			.SampleDesc = {.Count = 1, .Quality = 0 },
			.Flags = D3D12_RESOURCE_FLAG_NONE,
			};

			std::vector<D3D12_SUBRESOURCE_DATA> subresources;
			PrepareUpload(
				g_dxSample->m_device.Get(),
				image->GetImages(),
				image->GetImageCount(),
				image->GetMetadata(),
				subresources
			);

			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
			CD3DX12_RESOURCE_DESC resDesc(textureDesc);

			hr = g_dxSample->m_device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&m_res)
			);
			const auto uploadBufferSize = GetRequiredIntermediateSize(
				m_res.Get(),
				0,
				static_cast<uint32_t>(metadata.mipLevels)
			);

			const auto buf = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
			THROW_EXC_D3D(g_dxSample->m_device->CreateCommittedResource(
				&HEAP_TYPE_UPLOAD,
				D3D12_HEAP_FLAG_NONE,
				&buf,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(textureUploadHeap.GetAddressOf())),
				L"TexUpload"
			);

			UpdateSubresources(
				stateless.m_commandList.Get(),
				m_res.Get(),
				textureUploadHeap.Get(),
				0,
				0,
				static_cast<uint32_t>(metadata.mipLevels),
				&subresources[0]
			);
			stateless.m_commandList.Transition({ {
				m_res,
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE } }
				);
			CreateShaderResourceView(m_res->GetDesc());
			stateless.Commit();
		}

		Texture::Texture(
			const D3D12_RESOURCE_DESC& sourceFormat
		)
		{
			CreateShaderResourceView(sourceFormat);
		}

		Texture::Texture(
			uint32_t dx,
			uint32_t dy,
			DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags
		)
		{
			CreateShaderResource(*this, CD3DX12_RESOURCE_DESC::Tex2D(
				format,
				dx,
				dy,
				1u,
				1u,
				1,
				0,
				resourceFlags,
				D3D12_TEXTURE_LAYOUT_UNKNOWN, 0u)
			);

			CreateShaderResourceView(m_res->GetDesc());
		}

		Texture::Texture(
			OptionalUInt id,
			uint32_t dx,
			uint32_t dy,
			DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags,
			uint32_t sampleCount,
			D3D12_RESOURCE_STATES resourceState,
			D3D12_CLEAR_VALUE* clearValue
		) :
			m_onHeapID{ id }
		{
			CreateShaderResource(*this, CD3DX12_RESOURCE_DESC::Tex2D(
				format,
				dx,
				dy,
				1u,
				1u,
				sampleCount,
				0,
				resourceFlags),
				resourceState,
				clearValue);

			CreateShaderResourceView(m_res->GetDesc());
		}

		Texture::Texture(
			uint32_t dx,
			DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags)
		{
			CreateShaderResource(
				*this,
				CD3DX12_RESOURCE_DESC::Tex1D(
					format,
					dx,
					1u,
					1u,
					resourceFlags)
			);

			CreateShaderResourceView(m_res->GetDesc());
		}

		void Texture::CreateShaderResourceView(
			const D3D12_RESOURCE_DESC& resourceDesc
		)
		{
			if (!m_onHeapID)
				m_onHeapID = g_dxSample->m_cbSrCurrentID++;
			// Describe and create a SRV for the texture.
			const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = resourceDesc.Format ==
				DXGI_FORMAT_D32_FLOAT
				? DXGI_FORMAT_R32_FLOAT
				: resourceDesc.Format,
				.ViewDimension = ToViewDimension(resourceDesc),
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture1D = { 0, resourceDesc.MipLevels } };

			const CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = g_dxSample->
				m_cbvSrvHeap->GetCPUDescriptorHandle(*m_onHeapID);
			g_dxSample->m_device->CreateShaderResourceView(
				m_res.Get(),
				&srvDesc,
				cpuHandle);
		}

		Texture::Texture(
			const std::string& filename
		) :
			Texture{
			Repositories::g_repositoryGeneric->m_shaderResources[filename]
		}
		{}

		Texture::Texture(
			const SpanConstBytes sourceDDS
		) :
			Texture{
			sourceDDS.data(),
			sourceDDS.size()
		}
		{}

		void Texture::UpdateTextureInternal(
			const D3D12_SUBRESOURCE_DATA& textureData,
			ID3D12Resource* textureUploadHeap
		)
		{
			const auto scopedTransition = g_dxSample->m_commandList.ScopedTransition({ {
				m_res,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_COPY_DEST} }
				);
			UpdateSubresources(
				g_dxSample->m_commandList.Get(),
				m_res.Get(),
				textureUploadHeap,
				0,
				0,
				1,
				&textureData
			);
		}

		void Texture::SetMe(
			const ShaderConfigGeneric* shaderConfig,
			Registers::INPUT_RESOURCE baseShaderRegister
		) const
		{
			assert(nullptr != shaderConfig);
			shaderConfig->SetTableParameter(
				baseShaderRegister,
				g_dxSample->m_cbvSrvHeap->GetGPUDescriptorHandle(*m_onHeapID));
		}

		uint32_t Texture::GetOnHeapID() const noexcept
		{
			return *m_onHeapID;
		}

		static_assert(offsetof(D3D12_TEX1D_SRV, MipLevels) == offsetof(D3D12_TEX2D_SRV, MipLevels));
		static_assert(std::is_same<decltype(D3D12_TEX1D_SRV::MipLevels), decltype(D3D12_TEX2D_SRV::MipLevels)>::value);

		static_assert(std::is_copy_constructible_v<Texture>);
		static_assert(std::is_copy_assignable_v<Texture>);
		static_assert(std::is_nothrow_move_constructible_v<Texture>);
		static_assert(std::is_nothrow_move_assignable_v<Texture>);
	}
}