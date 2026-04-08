// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <memory>
// Monad
#include "Kernel/Del.h"
#include "Kernel/Events.h"
#include "__MonadRenderer.h"
#include "Pipeline.h"
#include "System/Console.h"

namespace Monad::Renderer::CB
{
	/// <summary>
	/// Resets the global constant-buffer group offset.
	/// </summary>
	void ZeroGroupOffset() noexcept;

	/// <summary>
	/// Constant buffer allocation mode.
	/// CB_STATE_SINGLE – single, immutable buffer (initialized once).
	/// CB_STATE_PAIR   – double-buffered (front/back) for swap chains.
	/// </summary>
	enum class CB_STATES
	{
		CB_STATE_PAIR,
		CB_STATE_SINGLE
	};

	struct CBInstanceGeneric;

	/// <summary>
	/// Generic controller for constant buffer types.
	/// Manages allocation, mapping, and lifetime of GPU constant buffers.
	/// </summary>
	interface CBTypeCtrlGeneric abstract : Kernel::EventD3DCreateStub
	{
		friend CBInstanceGeneric;

	protected:
		CBTypeCtrlGeneric(
			size_t length,
			uint32_t count
		);
		OPER_DEL(CBTypeCtrlGeneric);

	public:
		/// <summary>
		/// Returns the number of constant buffer instances managed.
		/// </summary>
		uint32_t GetCount() const noexcept;

	private:
		bool OnD3DCreateStub() final;
		void Unmap() noexcept;
		void Remap();
		template<typename Self>
		decltype(auto) GetGPUMemory(
			this Self&& self,
			uint32_t onHeapID,
			CB_STATES pair
		) noexcept
		{
			assert(std::forward<Self>(self).m_pCbvDataBegin);
			return std::forward<Self>(self).m_pCbvDataBegin
				+ static_cast<size_t>(std::forward<Self>(self).GetOffsetCB(onHeapID, pair) - self.c_heapStart)
				* static_cast<size_t>(std::forward<Self>(self).c_size256);
		}
		void SetMe(
			const ShaderConfigGeneric* shaderConfig,
			uint32_t onHeapID,
			Registers::CONSTANT_BUFFER baseShaderRegister,
			CB_STATES pair
		) noexcept;
		void Map(
			const D3D12_RANGE& range
		);
		const uint32_t GetOffsetCB(
			uint32_t onHeapID,
			CB_STATES pair
		) const noexcept;

	public:
		/// <summary>
		/// Allocation and usage counters for constant buffers.
		/// </summary>
		struct Count
		{
			bool IsFinished() const noexcept;
			bool IsFragmentedSwap() const noexcept;

			const uint32_t c_groupOffset, c_count;
			std::atomic_uint32_t
				m_currentOnHeap = 0u,
				m_currentOnTopHeap = c_count - 1u,
				m_counterOfUsages = 0u;
		} m_resCounter;

	private:
		const uint32_t c_size256, c_heapStart;
		PtrResource m_constantBuffer;
		uint8_t* m_pCbvDataBegin = nullptr;
	};

	template<typename C>
	struct CBTypeCtrl final : CBTypeCtrlGeneric
	{
		CBTypeCtrl(
			uint32_t count
		) noexcept :
			CBTypeCtrlGeneric{
				sizeof(C),
				count }
		{}
	};

	/// <summary>
	/// Base interface for a constant buffer instance.
	/// Represents one logical binding to a shader register.
	/// </summary>
	interface CBInstanceGeneric abstract
	{
		CBInstanceGeneric(
			const std::string & counter,
			CB_STATES pair,
			Registers::CONSTANT_BUFFER baseShaderRegister
		);
		virtual ~CBInstanceGeneric() = default;
		OPER_DEL(CBInstanceGeneric);
		/// <summary>
		/// Returns the size of the underlying constant buffer.
		/// </summary>
		virtual size_t SizeOfBuffer() const noexcept = 0;

		template<typename Self>
		decltype(auto) GetGPUMemory(this Self&& self) noexcept
		{
			return std::forward<Self>(self).m_typedCounter.GetGPUMemory(
				std::forward<Self>(self).c_onHeapID,
				std::forward<Self>(self).c_pair
			);
		}

		void GetCurrentCounterGlobal(
			const CB_STATES pair
		) noexcept;
		void SetMe(
			const ShaderConfigGeneric* shader
		) noexcept;

	protected:
		/// <summary>
		/// Every Constant Buffer for this register allocated
		/// </summary>
		/// <returns>true if all</returns>
		bool IsFinished() const noexcept;
		/// <summary>
		/// Maps modifiable Constant Buffers only
		/// </summary>
		void Remap();
		void Unmap() noexcept;
		bool IfOnlyConsts() const noexcept;
		bool IsFragmentedSwap() const noexcept;
		inline uint32_t GetMyHeap() const noexcept;
		void OutputRemap()
		{
#if defined _DEBUG
			std::wstring out =
				L"\nCBlocks modifiable: "
				+ std::to_wstring(m_typedCounter.m_resCounter.m_currentOnHeap)
				+ L" from: "
				+ std::to_wstring(m_typedCounter.m_resCounter.c_count)
				+ L"\n";
			System::MndOutputDebugString(out);
#endif
		}

		void IsFinishedAndAssigned()
		{
			if (IsFinished())
			{
				// Only consts, not for swap, then Unmap whole array
				if (IfOnlyConsts())
					Unmap();
				else
					Remap();
				OutputRemap();
			}
		}

		void IsFinishedAndNotAssigned()
		{
			if (IsFinished())
			{
				// Exclude consts
				if (IsFragmentedSwap())
					Remap();
				OutputRemap();
			}
		}

		CBTypeCtrlGeneric& m_typedCounter;
		const CB_STATES c_pair;
		const Registers::CONSTANT_BUFFER c_baseShaderRegister;
		const uint32_t c_onHeapID;
	};

	/// <summary>
	/// A wrapper for a Constant Buffer.
	/// It derives functionality from the CBInstanceGeneric
	/// and has a specialization to concrete Constant Buffer type.
	/// example HLSL:<para/>
	/// cbuffer cModel : register(b0)<para/>
	/// { <para/>
	/// float4x4 m_model;<para/>
	/// } <para/>
	/// example pseudo code for C++:<para/>
	/// using MyModelCB = CBGeneric &lt;cModel&gt;
	/// </summary>
	/// <typeparameter name="C">A struct passed to the shader</typeparameter>
	template<typename C>
	struct CBInstance : CBInstanceGeneric
	{
		using BUFFER_TYPE = C;
		using CONST_BUFFER_TYPE = const C;

		/// <summary>
		/// Immutable (single) constant buffer.
		/// </summary>
		/// <param name="counter">The index of C</param>
		/// <param name="cbType">The buffer's content</param>
		/// <param name="baseShaderRegister">The register</param>
		CBInstance(
			const std::string& counter,
			const BUFFER_TYPE& cbType,
			Registers::CONSTANT_BUFFER baseShaderRegister
		) :
			CBInstanceGeneric{ counter, CB_STATES::CB_STATE_SINGLE, baseShaderRegister }
#if defined _DEBUG
			, m_counter(counter)
#endif
		{
			GetBufferInternal() = cbType;
			IsFinishedAndAssigned();
		}

		/// <summary>
		/// Double-buffered (front/back) constant buffer.
		/// Content must be updated externally.
		/// </summary>
		/// <param name="counter">The index of C</param>
		/// <param name="baseShaderRegister">The register</param>
		CBInstance(
			const std::string& counter,
			Registers::CONSTANT_BUFFER baseShaderRegister) :
			CBInstanceGeneric{ counter, CB_STATES::CB_STATE_PAIR, baseShaderRegister }
#if defined _DEBUG
			, m_counter(counter)
#endif
		{
			IsFinishedAndNotAssigned();
		}

		size_t SizeOfBuffer() const noexcept final
		{
			return sizeof(BUFFER_TYPE);
		}

		template<typename Self>
		decltype(auto) GetBuffer(this Self&& self) noexcept
		{
			return std::forward<Self>(self).GetBufferInternal();
		}

	private:
		BUFFER_TYPE& GetBufferInternal() noexcept
		{
			return *reinterpret_cast<BUFFER_TYPE*>(this->GetGPUMemory());
		}

#if defined _DEBUG
		const std::string m_counter;
#endif
	};

	/// <summary>
	/// A wrapper for a Constant Buffer.
	/// It derives functionality from the CBInstanceGeneric
	/// and has a specialization to concrete Constant Buffer type.
	/// example HLSL:<para/>
	/// cbuffer cModel : register(b0)<para/>
	/// { <para/>
	/// float4x4 m_model;<para/>
	/// } <para/>
	/// example pseudo code for C++:<para/>
	/// using MyModelCB = CBBoundRegInstance &lt;cModel, b0&gt;
	/// </summary>
	/// <typeparameter name="C">A struct passed to the shader</typeparameter>
	template<typename CB, Registers::CONSTANT_BUFFER R>
	struct CBBoundRegInstance : CBInstance<CB>
	{
		/// <summary>
		/// Constant Constant Buffer
		/// </summary>
		/// <param name="counter">The index of C</param>
		/// <param name="cbType">The buffer's content</param>
		CBBoundRegInstance(
			const std::string& counter,
			const CB& cbType) :
			CBInstance<CB>{ counter, cbType, R }
		{
		}

		/// <summary>
		/// 2 Variable Constant Buffers for Back Buffer and Front Buffer
		/// They must be modified externally, they are not initialized for usage
		/// </summary>
		/// <param name="counter">The index of C</param>
		CBBoundRegInstance(
			const std::string& counter) :
			CBInstance<CB>{ counter, R }
		{
		}
	};

	using MapPtrConstantBufferManager = Kernel::UnorderedMapString<std::shared_ptr<CBTypeCtrlGeneric>>;

#define REG_CB(STRUCT_NAME) \
	using STRUCT_NAME##Instance = Renderer::CB::CBInstance<STRUCT_NAME>;\
	using STRUCT_NAME##Ctrl = Renderer::CB::CBTypeCtrl<STRUCT_NAME##Generic>;

#define REG_DEF_CB(STRUCT_NAME,REGISTER_ID) \
	using STRUCT_NAME##Instance = Renderer::CB::CBBoundRegInstance<STRUCT_NAME,REGISTER_ID>;\
	using STRUCT_NAME##Ctrl = Renderer::CB::CBTypeCtrl<STRUCT_NAME##Instance>;

	using HDR = BOOL;
	REG_DEF_CB(HDR, Registers::CONSTANT_BUFFER::B0);

	using LightPos = DirectX::XMFLOAT4;
	REG_DEF_CB(LightPos, Registers::CONSTANT_BUFFER::B1);

	using Shadow = DirectX::XMFLOAT4X4;
	REG_DEF_CB(Shadow, Registers::CONSTANT_BUFFER::B2);

	struct Model
	{
		DirectX::XMFLOAT4X4 m_modelViewProj, m_model;
	};
	REG_DEF_CB(Model, Registers::CONSTANT_BUFFER::B3);

	REG_DEF_CB(PipeColor, Registers::CONSTANT_BUFFER::B4);

	using Forecolor = DirectX::XMFLOAT3;
	REG_DEF_CB(Forecolor, Registers::CONSTANT_BUFFER::B5);
}
#define MonadHDR "hdr"_constantBuffer, make_shared<Monad::Renderer::CB::HDRCtrl>(1u)