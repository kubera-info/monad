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
	struct CBTypeCtrl final : Kernel::EventD3DCreateStub
	{
		friend CBInstanceGeneric;

		CBTypeCtrl(
			size_t length,
			uint32_t count
		);
		CBTypeCtrl() noexcept;
		CBTypeCtrl(
			const CBTypeCtrl& source
		) = default;
		CBTypeCtrl& operator=(
			const CBTypeCtrl& source
			) = default;
		CBTypeCtrl(
			CBTypeCtrl&& source
		) noexcept;
		CBTypeCtrl& operator=(
			CBTypeCtrl&& source
			) noexcept;
		void Move(CBTypeCtrl&& source) noexcept;

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
			assert(std::forward<Self>(self).m_pCbvDataBegin && "Controller of Constant Buffer must not be nullptr.");
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

			uint32_t
				m_groupOffset,
				m_count,
				m_currentOnHeap = 0u,
				m_currentOnTopHeap = m_count - 1u,
				m_counterOfUsages = 0u;
		} m_resCounter;
		uint32_t c_size256;

	private:
		uint32_t c_heapStart;
		uint8_t* m_pCbvDataBegin;
		PtrResource m_constantBuffer;
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
				+ std::to_wstring(m_typedCounter.m_resCounter.m_count)
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

		CBTypeCtrl& m_typedCounter;
		Registers::CONSTANT_BUFFER c_baseShaderRegister;
		CB_STATES c_pair;
		uint32_t c_onHeapID;
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
			assert(((sizeof(BUFFER_TYPE)
				+ (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1))
				& ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) == this->m_typedCounter.c_size256);
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

	struct MapConstantBufferManager final : private Kernel::FlatMapString<CBTypeCtrl>
	{
		using value_type = typename Kernel::FlatMapString<CBTypeCtrl>::value_type;
		using InitializerListConstantBufferManager = std::initializer_list<value_type>;
		using Kernel::FlatMapString<CBTypeCtrl>::FlatMapString;

		// Map operations
		[[nodiscard]] iterator find(const key_type& _Key_val);
		[[nodiscard]] const_iterator cbegin() const noexcept;
		[[nodiscard]] const_iterator cend() const noexcept;
		[[nodiscard]] iterator begin() noexcept;
		[[nodiscard]] iterator end() noexcept;
	};

	using HDRInstance = CBBoundRegInstance<BOOL, Registers::CONSTANT_BUFFER::B0>;

	using LightPosInstance = CBBoundRegInstance<DirectX::XMFLOAT4, Registers::CONSTANT_BUFFER::B1>;

	using ShadowInstance = CBBoundRegInstance<DirectX::XMFLOAT4X4, Registers::CONSTANT_BUFFER::B2>;

	struct Model
	{
		DirectX::XMFLOAT4X4 m_modelViewProj, m_model;
	};
	using ModelInstance = CBBoundRegInstance<Model, Registers::CONSTANT_BUFFER::B3>;;

	using PipeColorInstance = CBBoundRegInstance<PipeColor, Registers::CONSTANT_BUFFER::B4>;

	using ForecolorInstance = CBBoundRegInstance<DirectX::XMFLOAT3, Registers::CONSTANT_BUFFER::B5>;
}