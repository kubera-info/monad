// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Pipeline.h"
// STD
#include <ranges>
// Monad
#include "Core/DXSample.h"
#include "Core/DXSampleHelper.h"
#include "States.h"
#include "Repository/Repo.h"

using namespace std;

namespace Monad
{
	using namespace Kernel;
	using namespace Repositories;

	namespace Renderer
	{
		namespace
		{
			constexpr D3D12_ROOT_SIGNATURE_FLAGS ROOT_SIGNATURE_FLAGS =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
		}
#pragma region InitD3D12Ranges1
		ShaderConfigGeneric::InitD3D12Ranges1::InitD3D12Ranges1(
#if defined _DEBUG
			const wstring& name,
#endif
			InitD3D12Ranges1::InitializerListInitRanges ranges
		)
#if defined _DEBUG
			: m_name(name)
#endif
		{
			// These structures are simplified. (assumed m_ranges.size() == rootParameters.size()).
			m_ranges.reserve(ranges.size());
			vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
			rootParameters.reserve(ranges.size());
			for (const auto& oneTuple : ranges)
				rootParameters.emplace_back().InitAsDescriptorTable(
					1,
					&m_ranges.emplace_back(
						oneTuple.m_rangeType,
						1,
						oneTuple.m_baseShaderRegister,
						0,
						oneTuple.m_rangeFlags),
					oneTuple.m_shaderVisibility);
			g_dxSample->CreateOneRootSignature(
#if defined _DEBUG
				name,
#endif
				{
					static_cast<uint32_t>(rootParameters.size()),
					rootParameters.data(),
					g_samplers.GetAllSamplersCount(),
					g_samplers.GetAllSamplersDesc(),
					ROOT_SIGNATURE_FLAGS
				},
				m_rootSignature
			);
		}

		void ShaderConfigGeneric::InitD3D12Ranges1::SetTableParameter(
			const D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
			const uint32_t baseShaderRegister,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept
		{
			g_dxSample->m_commandList.Get()->
				SetGraphicsRootDescriptorTable(
					QueryTableParameter(
						rangeType,
						baseShaderRegister
					),
					gpuHandle
				);
		}

		uint32_t ShaderConfigGeneric::InitD3D12Ranges1::QueryTableParameter(
			const D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
			const uint32_t baseShaderRegister
		) const noexcept
		{
			for (const auto [index, range] : m_ranges | std::views::enumerate)
				// Range in Root Parameter, it is enough
				if (baseShaderRegister == range.BaseShaderRegister
					&& rangeType == range.RangeType)
				{
					assert(0 == range.RegisterSpace);
					__assume(0 == range.RegisterSpace);
					return static_cast<uint32_t>(index);
				}
#if defined _DEBUG
			OutputDebugString(L"Parameter not found: ");
			OutputDebugString((
				m_name
				+ L" rangeType-"
				+ to_wstring(rangeType)
				+ L" baseShaderRegister-"
				+ to_wstring(baseShaderRegister)).c_str());
			OutputDebugString(L"\n");
#endif
			std::unreachable();
		}
#pragma endregion

#pragma region ShaderConfigGeneric::InitD3D12Ranges1::InitD3D12Range1
		ShaderConfigGeneric::InitD3D12Ranges1::InitD3D12Range1::InitD3D12Range1(
			const Registers::INPUT_RESOURCE baseShaderRegister
		) noexcept :
			InitD3D12Range1{
			D3D12_SHADER_VISIBILITY_PIXEL,
			D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			static_cast<uint32_t>(baseShaderRegister)
		}
		{}

		ShaderConfigGeneric::InitD3D12Ranges1::InitD3D12Range1::InitD3D12Range1(
			const D3D12_SHADER_VISIBILITY shaderVisibility,
			const Registers::CONSTANT_BUFFER baseShaderRegister
		) noexcept :
			InitD3D12Range1{
			shaderVisibility,
			D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			static_cast<uint32_t>(baseShaderRegister)
		}
		{}

		ShaderConfigGeneric::InitD3D12Ranges1::InitD3D12Range1::InitD3D12Range1(
			const D3D12_SHADER_VISIBILITY shaderVisibility,
			const D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
			const uint32_t baseShaderRegister
		) noexcept :
			m_shaderVisibility(shaderVisibility),
			m_rangeType(rangeType),
			m_baseShaderRegister(baseShaderRegister),
			m_rangeFlags(D3D12_DESCRIPTOR_RANGE_FLAG_NONE)
		{}
#pragma endregion

#pragma region ShaderConfigGeneric
		ShaderConfigGeneric::ShaderConfigGeneric(
			const string& vs,
			const string& ps,
			bool alphaBlending
		) noexcept :
			m_vs{ vs },
			m_ps{ ps },
			m_alphaBlending{ alphaBlending }
		{}

		void ShaderConfigGeneric::insert(
			const string& leadingTechID,
			InitD3D12Ranges1::InitializerListInitRanges ranges
		)
		{
#if defined _DEBUG
			if (!g_dxSample->m_rootSignatures.contains(leadingTechID))
			{
				OutputDebugString(L"\nNew rootSiganture: ");
				OutputDebugStringA(leadingTechID.c_str());
				OutputDebugString(L"\n");
			}
#endif
			m_parametersDef = &g_dxSample->m_rootSignatures.try_emplace(
				leadingTechID,
#if defined _DEBUG
				Monad::Tools::UTF8ToUnicode(leadingTechID),
#endif
				ranges
			).first->second;
		}

		void ShaderConfigGeneric::CreateGraphicsPipelineStateGeneric(
			const string& id
		)
		{
			if (!m_pipelineState)
			{
				const auto psoDesc = CreateGraphicsPipelineStateDesc(
					id);
				THROW_EXC_D3D(g_dxSample->m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)), L"Pipeline State");
#if defined _DEBUG
				const wstring name = L"Pipeline-State-" + Monad::Tools::UTF8ToUnicode(id);
				SetName(m_pipelineState.Get(), name.data());
#endif
			}
		}

		void ShaderConfigGeneric::SetMe() const noexcept
		{
			assert(m_pipelineState);
			assert(nullptr != m_parametersDef);
			assert(m_parametersDef->m_rootSignature);
			g_dxSample->m_commandList.SetMe(
				m_pipelineState.Get(),
				m_parametersDef->m_rootSignature.Get()
			);
		}

		uint32_t ShaderConfigGeneric::QueryTableParameter(
			Registers::CONSTANT_BUFFER baseShaderRegister
		) const noexcept
		{
			assert(m_parametersDef);
			return m_parametersDef->QueryTableParameter(
				D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
				static_cast<uint32_t>(baseShaderRegister)
			);
		}
		uint32_t ShaderConfigGeneric::QueryTableParameter(
			Registers::INPUT_RESOURCE baseShaderRegister
		) const noexcept
		{
			assert(m_parametersDef);
			return m_parametersDef->QueryTableParameter(
				D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				static_cast<uint32_t>(baseShaderRegister)
			);
		}

		void ShaderConfigGeneric::SetTableParameter(
			Registers::CONSTANT_BUFFER baseShaderRegister,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept
		{
			assert(m_parametersDef);
			m_parametersDef->SetTableParameter(
				D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
				static_cast<uint32_t>(baseShaderRegister),
				gpuHandle
			);
		}
		void ShaderConfigGeneric::SetTableParameter(
			Registers::INPUT_RESOURCE baseShaderRegister,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept
		{
			assert(m_parametersDef);
			m_parametersDef->SetTableParameter(
				D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				static_cast<uint32_t>(baseShaderRegister),
				gpuHandle
			);
		}
		void ShaderConfigGeneric::SetTableParameter(
			uint32_t tableID,
			const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
		) const noexcept
		{
			g_dxSample->m_commandList.Get()->
				SetGraphicsRootDescriptorTable(
					tableID,
					gpuHandle
				);
		}
#pragma endregion

#pragma region MapFX
		ShaderConfigGeneric* MapFX::GetPipeline(
			const std::string& id
		)
		{
			auto& tech = operator[](id);
			tech->CreateGraphicsPipelineStateGeneric(id);
			return tech.get();
		}
#pragma endregion

#pragma region SetIfThis
		SetIfThis::const_iterator SetIfThis::IfClass(
			const std::string& id
		) const noexcept
		{
			return contains(id) ? cbegin() : cend();
		}
#pragma endregion
	}
}