// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Pipeline.h"
#include "Kernel/Del.h"

namespace Monad::Renderer
{
	/// <summary>
	/// A class for a texture implementation
	/// </summary>
	struct Texture final
	{
		Texture() = default;
		~Texture() = default;
		Texture(
			const D3D12_RESOURCE_DESC& sourceFormat
		);
		Texture(
			uint32_t dx,
			uint32_t dy,
			DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags
		);
		Texture(
			OptionalUInt id,
			uint32_t dx,
			uint32_t dy,
			DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags,
			uint32_t sampleCount = 1,
			D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_CLEAR_VALUE* clearValue = nullptr
		);
		Texture(
			uint32_t dx,
			DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags
		);
		Texture(
			const std::string& filename
		);
		Texture(
			const Kernel::SpanConstBytes sourceDDS
		);
		void SetMe(
			const ShaderConfigGeneric* shaderConfig,
			Registers::INPUT_RESOURCE baseShaderRegister
		) const;
		OPER_DEF_NO_DEF_CTOR(Texture);

		template<typename V>
		void UpdateTexture1D(
			const V& source,
			ID3D12Resource* textureUploadHeap
		)
		{
			const auto sizeOfTexture = static_cast<LONG_PTR>(source.size() * sizeof(V::value_type));
			UpdateTextureInternal({
				source.data(),
				sizeOfTexture,
				sizeOfTexture
				},
				textureUploadHeap
			);
		}

		template<typename V>
		inline void UpdateTexture1D(
			const V& source,
			const PtrResource& textureUploadHeap
		)
		{
			UpdateTexture1D(
				source,
				textureUploadHeap.Get()
			);
		}

		uint32_t GetOnHeapID() const noexcept;

		PtrResource m_res;

	private:
		Texture(
			const uint8_t* source,
			size_t sourceSize
		);
		void CreateShaderResourceView(
			const D3D12_RESOURCE_DESC& resourceDesc
		);
		void UpdateTextureInternal(
			const D3D12_SUBRESOURCE_DATA& textureData,
			ID3D12Resource* textureUploadHeap
		);

		OptionalUInt m_onHeapID;
	};
}