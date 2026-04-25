// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "ConstantBuffers.h"
// Monad
#include "Core/DXSample.h"
#include "Core/DXSampleHelper.h"
#include "Exceptions/Exceptions.h"
#include "__MonadRenderer.h"

namespace Monad::Renderer
{
	namespace CB
	{
		namespace
		{
			uint32_t g_groupOffset = 0;
		}

		void ZeroGroupOffset() noexcept
		{
			g_groupOffset = 0;
		}

#pragma region CBTypeCtrl
		CBTypeCtrl::CBTypeCtrl(
			size_t length,
			uint32_t count
		) :
			m_resCounter{
				g_groupOffset,
				count
		},
			c_size256{ (length
				+ (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1))
				& ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1) },
			c_heapStart{
					m_resCounter.m_currentOnHeap
					+ m_resCounter.m_groupOffset
				},
			m_pCbvDataBegin(nullptr)
		{
			Register();
			g_groupOffset += count;
		}
		
		CBTypeCtrl::CBTypeCtrl() noexcept :
			c_size256(256),
			c_heapStart(0),
			m_pCbvDataBegin(nullptr)
		{
		}

		CBTypeCtrl::CBTypeCtrl(
			CBTypeCtrl&& source
		) noexcept
		{
			Move(std::move(source));
		}

		CBTypeCtrl& CBTypeCtrl::operator=(
			CBTypeCtrl&& source
			) noexcept
		{
			Move(std::move(source));

			return *this;
		}

		void CBTypeCtrl::Move(CBTypeCtrl&& source) noexcept
		{
			__super::MoveEvent(std::move(source));
			m_resCounter = source.m_resCounter;
			c_size256 = source.c_size256;
			c_heapStart = source.c_heapStart;
			m_pCbvDataBegin = source.m_pCbvDataBegin;
			source.m_pCbvDataBegin = nullptr;
			m_constantBuffer.Attach(source.m_constantBuffer.Detach());
		}
		
		void CBTypeCtrl::Unmap() noexcept
		{
			assert(m_pCbvDataBegin);
			assert(m_constantBuffer);
			m_constantBuffer->Unmap(
				0,
				nullptr
			);
			m_pCbvDataBegin = nullptr;
		}

		bool CBTypeCtrl::OnD3DCreateStub()
		{
			const auto buf = CD3DX12_RESOURCE_DESC::Buffer(
				static_cast<size_t>(c_size256)
				* static_cast<size_t>(m_resCounter.m_count)
			);
			THROW_EXC_D3D(g_dxSample->m_device->CreateCommittedResource(
				&HEAP_TYPE_UPLOAD,
				D3D12_HEAP_FLAG_NONE,
				&buf,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_constantBuffer)),
				L"Create Committed Resource"
			);
			NAME_D3D12_OBJECT(m_constantBuffer);

			// Describe and create a constant buffer view.
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{
				m_constantBuffer->GetGPUVirtualAddress(),
				c_size256
			};
			auto cpuHandle = g_dxSample->m_cbvSrvHeap->GetCPUDescriptorHandle(
				m_resCounter.m_groupOffset
				+ m_resCounter.m_currentOnHeap
			);
			for (uint32_t n = 0; n < m_resCounter.m_count; ++n)
			{
				g_dxSample->m_device->CreateConstantBufferView(
					&cbvDesc,
					cpuHandle
				);

				cbvDesc.BufferLocation += c_size256;
				cpuHandle.Offset(g_dxSample->m_cbvSrvHeap->GetDescriptorSize());
			}
			Map(WHOLE_RANGE);
			return true;
		}

		void CBTypeCtrl::Remap()
		{
			Unmap();
			Map({
				0,
				m_resCounter.m_currentOnHeap * c_size256 }
				);
		}

		uint32_t CBTypeCtrl::GetCount() const noexcept
		{
			return m_resCounter.m_count;
		}

		void CBTypeCtrl::Map(
			const D3D12_RANGE& range
		)
		{
			THROW_EXC_IFFALSE(Exceptions::ThreeDFailed, nullptr == m_pCbvDataBegin, L"Map CB");
			THROW_EXC_IFNULL(Exceptions::ThreeDFailed, m_constantBuffer, L"Map CB #2");
			THROW_EXC_D3D(m_constantBuffer->Map(0, &range, reinterpret_cast<void**>(&m_pCbvDataBegin)), L"Map CB #3");
		}

		const uint32_t CBTypeCtrl::GetOffsetCB(
			uint32_t onHeapID,
			CB_STATES pair
		) const noexcept
		{
			return
				onHeapID
				+ (CB_STATES::CB_STATE_PAIR == pair
					? g_frameIndex
					: 0u);
		}

		bool CBTypeCtrl::Count::IsFinished() const noexcept
		{
			return m_count == m_counterOfUsages;
		}

		bool CBTypeCtrl::Count::IsFragmentedSwap() const noexcept
		{
			return m_count != m_currentOnHeap;
		}
		void CBTypeCtrl::SetMe(
			const ShaderConfigGeneric* shaderConfig,
			uint32_t onHeapID,
			Registers::CONSTANT_BUFFER baseShaderRegister,
			CB_STATES pair
		) noexcept
		{
			assert(nullptr != shaderConfig);
			shaderConfig->SetTableParameter(
				baseShaderRegister,
				g_dxSample->m_cbvSrvHeap->GetGPUDescriptorHandle(GetOffsetCB(onHeapID, pair)));
		}
#pragma endregion

#pragma region CBInstanceGeneric
		CBInstanceGeneric::CBInstanceGeneric(
			const std::string& counter,
			CB_STATES pair,
			Registers::CONSTANT_BUFFER baseShaderRegister
		) :
			m_typedCounter(
				g_dxSample->m_constantBufferManager.find(counter)->second),
			c_baseShaderRegister(baseShaderRegister),
			c_pair(pair),
			c_onHeapID(GetMyHeap())
		{
			GetCurrentCounterGlobal(pair);
		}

		void CBInstanceGeneric::GetCurrentCounterGlobal(
			const CB_STATES pair
		) noexcept
		{
			if (CB_STATES::CB_STATE_PAIR == pair)
				m_typedCounter.m_resCounter.m_counterOfUsages += 2;
			else
				m_typedCounter.m_resCounter.m_counterOfUsages++;
		}

		inline uint32_t CBInstanceGeneric::GetMyHeap() const noexcept
		{
			auto& heapTypeDefault = m_typedCounter.m_resCounter;
			if (CB_STATES::CB_STATE_PAIR == c_pair)
			{
				/* Swappable variables at the bottom */
				uint32_t ret = heapTypeDefault.m_groupOffset + heapTypeDefault.m_currentOnHeap;
				heapTypeDefault.m_currentOnHeap += 2;
				return ret;
			}
			else
				/* Consts at the top */
				return heapTypeDefault.m_groupOffset + heapTypeDefault.m_currentOnTopHeap--;
		}

		void CBInstanceGeneric::SetMe(
			const ShaderConfigGeneric* shader
		) noexcept
		{
			m_typedCounter.SetMe(
				shader,
				c_onHeapID,
				c_baseShaderRegister,
				c_pair
			);
		}

		bool CBInstanceGeneric::IsFinished() const noexcept
		{
			return m_typedCounter.m_resCounter.IsFinished();
		}

		void CBInstanceGeneric::Remap()
		{
			m_typedCounter.Remap();
		}

		void CBInstanceGeneric::Unmap() noexcept
		{
			m_typedCounter.Unmap();
		}

		bool CBInstanceGeneric::IfOnlyConsts() const noexcept
		{
			return c_onHeapID == m_typedCounter.m_resCounter.m_groupOffset;
		}

		bool CBInstanceGeneric::IsFragmentedSwap() const noexcept
		{
			return m_typedCounter.m_resCounter.IsFragmentedSwap();
		}

		MapConstantBufferManager::iterator MapConstantBufferManager::find(
			const MapConstantBufferManager::key_type& _Key_val
		)
		{
			auto manager = Kernel::FlatMapString<CBTypeCtrl>::find(_Key_val);
			THROW_EXC_ONEND(*this, manager, L"CB Manager");
			return manager;
		}

		MapConstantBufferManager::const_iterator MapConstantBufferManager::cbegin() const noexcept
		{
			return Kernel::FlatMapString<CBTypeCtrl>::cbegin();
		}

		MapConstantBufferManager::const_iterator MapConstantBufferManager::cend() const noexcept
		{
			return Kernel::FlatMapString<CBTypeCtrl>::cend();
		}

		MapConstantBufferManager::iterator MapConstantBufferManager::begin() noexcept
		{
			return Kernel::FlatMapString<CBTypeCtrl>::begin();
		}

		MapConstantBufferManager::iterator MapConstantBufferManager::end() noexcept
		{
			return Kernel::FlatMapString<CBTypeCtrl>::end();
		}
#pragma endregion
	}
}