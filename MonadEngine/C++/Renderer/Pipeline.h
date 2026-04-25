// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include <d3dx12.h>
// STD
#include <memory>
// Monad
#include "__MonadRenderer.h"
#include "Kernel/Templates.h"

namespace Monad::Renderer
{
	// =========================================================================
	// Technique
	// =========================================================================

	struct Technique
	{
		std::string m_worldState;

#if MONAD_SHADOW
		Technique(
			const std::string worldState,
			const OptionalString& shadowState = std::nullopt
		) noexcept :
			m_worldState(worldState),
			m_shadowState(shadowState)
		{}
		OptionalString m_shadowState;
#else
		Technique(
			const std::string& worldState
		) :
			m_worldState(worldState)
		{}
#endif
	};

	// =========================================================================
	// Shader Configuration
	// =========================================================================

	interface ShaderConfigGeneric abstract
	{
		// ---------------------------------------------------------------------
		// Root Signature Builder
		// ---------------------------------------------------------------------

	struct InitD3D12Ranges1 final
			{
				struct InitD3D12Range1
				{
					friend InitD3D12Ranges1;

					/// <summary>
					/// Shader Resource View
					/// </summary>
					/// <param name="baseShaderRegister">ID of resurce view</param>
					InitD3D12Range1(
						const Registers::INPUT_RESOURCE baseShaderRegister
					) noexcept;
					/// <summary>
					/// Constant Buffer View
					/// </summary>
					/// <param name="shaderVisibility">VS/PS</param>
					/// <param name="index">The ID of block</param>
					InitD3D12Range1(
						const D3D12_SHADER_VISIBILITY shaderVisibility,
						const Registers::CONSTANT_BUFFER baseShaderRegister
					) noexcept;

				private:
					InitD3D12Range1(
						const D3D12_SHADER_VISIBILITY shaderVisibility,
						const D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
						const uint32_t baseShaderRegister
					) noexcept;

					const D3D12_SHADER_VISIBILITY m_shaderVisibility;
					const D3D12_DESCRIPTOR_RANGE_TYPE m_rangeType;
					const uint32_t m_baseShaderRegister;
					const D3D12_DESCRIPTOR_RANGE_FLAGS m_rangeFlags;
				};
				using InitializerListInitRanges = std::initializer_list<InitD3D12Range1>;
				InitD3D12Ranges1(
#if defined _DEBUG
					const std::wstring& name,
#endif
					InitializerListInitRanges ranges
				);
				void SetTableParameter(
					const D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
					const uint32_t baseShaderRegister,
					const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
				) const noexcept;
				uint32_t QueryTableParameter(
					const D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
					const uint32_t baseShaderRegister
				) const noexcept;

				PtrRootSignature m_rootSignature;

				std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_ranges;
#if defined _DEBUG
			private:
				std::wstring m_name;
#endif
			};

	// ---------------------------------------------------------------------
// ShaderConfigGeneric interface
// ---------------------------------------------------------------------

public:
		using MapRootSignatures = Kernel::UnorderedMapString<InitD3D12Ranges1>;
		ShaderConfigGeneric(
			const std::string& vs,
			const std::string& ps,
			bool pAlphaBlending) noexcept;
		void insert(
			const std::string& leadingTechID,
			InitD3D12Ranges1::InitializerListInitRanges ranges
		);
		void CreateGraphicsPipelineStateGeneric(
			const std::string& id
		);
		void SetMe() const noexcept;
		uint32_t QueryTableParameter(
			Registers::CONSTANT_BUFFER baseShaderRegister
		) const noexcept;
		uint32_t QueryTableParameter(
			Registers::INPUT_RESOURCE baseShaderRegister
		) const noexcept;
		void SetTableParameter(
			Registers::CONSTANT_BUFFER baseShaderRegister,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept;
		void SetTableParameter(
			Registers::INPUT_RESOURCE baseShaderRegister,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept;
		void SetTableParameter(
			uint32_t tableID,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept;

	protected:
		std::string m_vs, m_ps;

	public:
		bool m_alphaBlending;

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

	protected:
		virtual const D3D12_GRAPHICS_PIPELINE_STATE_DESC CreateGraphicsPipelineStateDesc(
			const std::string& id
		) = 0;

		InitD3D12Ranges1* m_parametersDef = nullptr;
	};
	using OptionalShaderConfig = std::optional<ShaderConfigGeneric*>;

	// =========================================================================
	// Shader maps
	// =========================================================================

	struct MapFX : Kernel::FlatMapString<std::shared_ptr<ShaderConfigGeneric>>
	{
		using flat_map::flat_map;
		using InitializerListFXes = std::initializer_list<value_type>;

		ShaderConfigGeneric* GetPipeline(
			const std::string& id
		);
	};


	// =========================================================================
	// Utility
	// =========================================================================

	/// <summary>
	/// A helper set that checks whether an identifier belongs
	/// to a known class group.
	/// </summary>
	struct SetIfThis : std::set<std::string>
	{
		using set::set;

		const_iterator IfClass(
			const std::string& id
		) const noexcept;
	};
}

// =========================================================================
// Application-side specialization
// =========================================================================

namespace Monad::App
{
	struct ShaderConfig final : Renderer::ShaderConfigGeneric
	{
		using ShaderConfigGeneric::ShaderConfigGeneric;

	private:
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC CreateGraphicsPipelineStateDesc(
			const std::string& id
		) final;
	};
}