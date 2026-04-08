// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl.h>
// STD
#include <optional>
#include <string>
#include <stdint.h>
// Monad
#include "Kernel/Del.h"
#include "Kernel/BytesVector.h"

namespace Monad::Renderer
{
	using PtrD3D12Device = Microsoft::WRL::ComPtr<ID3D12Device>;
	using PtrResource = Microsoft::WRL::ComPtr<ID3D12Resource>;
	using PtrRootSignature = Microsoft::WRL::ComPtr<ID3D12RootSignature>;
	using OptionalUInt = std::optional<uint32_t>;
	using OptionalWString = std::optional<std::wstring>;
	using OptionalString = std::optional<std::string>;
	using VectorIndices16 = Kernel::VectorNoCtor<uint16_t>;
	using VectorIndices32 = Kernel::VectorNoCtor<uint32_t>;

	extern uint32_t g_frameIndex;

#if defined _DEBUG
	constexpr uint32_t DXGI_FACTORY_FLAGS = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr uint32_t DXGI_FACTORY_FLAGS = 0u;
#endif

	constexpr float MAX_BYTE(0xff);
	inline constexpr DirectX::XMFLOAT4 FLOAT_4_ONES{ 1.0f, 1.0f, 1.0f, 1.0f };

	interface FormatAndClear abstract
	{
		D3D12_CLEAR_VALUE m_clearValue;
	};

	struct FormatAndClearRenderTarget final : public FormatAndClear
	{
		FormatAndClearRenderTarget(
			const DXGI_FORMAT format,
			const DirectX::XMFLOAT4 color,
			const D3D12_RTV_DIMENSION viewDimension
		) noexcept :
			FormatAndClear{ {.Format = format, .Color{ color.x, color.y, color.z, color.w } } },
			m_desc{ .Format = format, .ViewDimension = viewDimension }
		{}
		OPER_DEF_NO_DEF_CTOR(FormatAndClearRenderTarget);
		FormatAndClearRenderTarget() noexcept = default;
		~FormatAndClearRenderTarget() = default;

		D3D12_RENDER_TARGET_VIEW_DESC m_desc;
	};

	struct FormatAndClearDepthStencil final : public FormatAndClear
	{
		consteval FormatAndClearDepthStencil(
			const D3D12_DSV_DIMENSION viewDimension
		) noexcept :
			FormatAndClear{ {.Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil{.Depth = 1.0f, .Stencil = 0 } } },
			m_desc{ .Format = DXGI_FORMAT_D32_FLOAT, .ViewDimension = viewDimension }
		{}

		const D3D12_DEPTH_STENCIL_VIEW_DESC m_desc;
	} constexpr
		DEPTH_STENCIL_VIEW_DESC{ D3D12_DSV_DIMENSION_TEXTURE2D },
		DEPTH_STENCIL_VIEW_DESC_MSAA{ D3D12_DSV_DIMENSION_TEXTURE2DMS };

	inline constexpr D3D12_HEAP_PROPERTIES
		HEAP_TYPE_DEFAULT{ .Type = D3D12_HEAP_TYPE_DEFAULT, .CreationNodeMask = 1, .VisibleNodeMask = 1 },
		HEAP_TYPE_UPLOAD{ .Type = D3D12_HEAP_TYPE_UPLOAD, .CreationNodeMask = 1, .VisibleNodeMask = 1 };

	constexpr D3D12_RANGE WHOLE_RANGE{ 0, 0 };

	constexpr D3D12_RESOURCE_DESC RESOURCE_DESC{
	.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
	.MipLevels = 1,
	.Format = DXGI_FORMAT_R8G8B8A8_UNORM
	};

	// Frontbuffer + Backbuffer
	constexpr uint32_t FRAME_COUNT = 2;

	// Describe and create the command queue.
	inline constexpr D3D12_COMMAND_QUEUE_DESC COMMAND_QUEUE_DESC{
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		D3D12_COMMAND_QUEUE_FLAG_NONE,
		0
	};

	// =========================================================================
	// Shader bytecode wrapper
	// =========================================================================

	struct VectorBytesBytecode : Kernel::VectorBytes
	{
		using VectorBytes::VectorBytes;
		VectorBytesBytecode(VectorBytes&& source) noexcept;
		/// <summary>
		/// Returns a compiled shader from the STL vector
		/// </summary>
		/// <returns>Bytecode</returns>
		operator const D3D12_SHADER_BYTECODE() const noexcept;
	};

	struct ConfigVS
	{
		const VectorBytesBytecode m_vs;
		const std::string c_il;
	};

	/*/// <summary>
	/// Converts 4 bytes into ARGB
	/// </summary>
	/// <param name="r">Red</param>
	/// <param name="g">Green</param>
	/// <param name="b">Blue</param>
	/// <returns>ARGB</returns>
	inline RGBA MND_D3DCOLOR_ARGB(const uint8_t a, const uint8_t r, const uint8_t g, const uint8_t b) noexcept
	{
		return (a << 24) | (r << 16) | (g << 8) | b;
	}*/

	/// <summary>
	/// Converts a float (0.0f .. 1.0f) to an uint8_t (0 .. 255)
	/// </summary>
	/// <param name="pChannel">Input float parameter</param>
	/// <returns>Unsigned byte</returns>
	constexpr uint8_t MND_MAKE_BYTE(
		const float channel
	) noexcept
	{
		return static_cast<uint8_t>(channel * MAX_BYTE);
	}

	constexpr float MND_MAKE_FLOAT0(
		const float channel
	) noexcept
	{
		return channel / MAX_BYTE;
	}

	constexpr float MND_MAKE_FLOAT(
		const uint8_t channel
	) noexcept
	{
		return MND_MAKE_FLOAT0(static_cast<float>(channel));
	}
	// =========================================================================
	// Register enums
	// =========================================================================


	namespace Registers
	{
		enum class CONSTANT_BUFFER : uint32_t
		{
			B0,
			B1,
			B2,
			B3,
			B4,
			B5,
			B6,
			B7,
			B8,
			B9,
			B10,
			B11,
			B12,
			B13
		};
		static_assert(static_cast<size_t>(CONSTANT_BUFFER::B13) + 1 == D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
		enum class INPUT_RESOURCE : uint32_t
		{
			T0,
			T1,
			T2,
			T3,
			T4,
			T5,
			T6,
			T7,
			T8,
			T9,
			T10,
			T11,
			T12,
			T13,
			T14,
			T15,
			T16,
			T17,
			T18,
			T19,
			T20,
			T21,
			T22,
			T23,
			T24,
			T25,
			T26,
			T27,
			T28,
			T29,
			T30,
			T31,
			T32,
			T33,
			T34,
			T35,
			T36,
			T37,
			T38,
			T39,
			T40,
			T41,
			T42,
			T43,
			T44,
			T45,
			T46,
			T47,
			T48,
			T49,
			T50,
			T51,
			T52,
			T53,
			T54,
			T55,
			T56,
			T57,
			T58,
			T59,
			T60,
			T61,
			T62,
			T63,
			T64,
			T65,
			T66,
			T67,
			T68,
			T69,
			T70,
			T71,
			T72,
			T73,
			T74,
			T75,
			T76,
			T77,
			T78,
			T79,
			T80,
			T81,
			T82,
			T83,
			T84,
			T85,
			T86,
			T87,
			T88,
			T89,
			T90,
			T91,
			T92,
			T93,
			T94,
			T95,
			T96,
			T97,
			T98,
			T99,
			T100,
			T101,
			T102,
			T103,
			T104,
			T105,
			T106,
			T107,
			T108,
			T109,
			T110,
			T111,
			T112,
			T113,
			T114,
			T115,
			T116,
			T117,
			T118,
			T119,
			T120,
			T121,
			T122,
			T123,
			T124,
			T125,
			T126,
			T127
		};
	}
	namespace InputLayout
	{
		enum class SEMANTIC_INDEX
		{
			SI0,
			SI1,
			SI2,
			SI3,
			SI4,
			SI5,
			SI6,
			SI7,
			SI8,
			SI9,
			SI10,
			SI11,
			SI12,
			SI13,
			SI14,
			SI15,
			SI16,
			SI17,
			SI18,
			SI19,
			SI20,
			SI21,
			SI22,
			SI23,
			SI24,
			SI25,
			SI26,
			SI27,
			SI28,
			SI29,
			SI30,
			SI31
		};
	}
	namespace CB
	{
		struct PipeColor
		{
			DirectX::XMFLOAT4 m_diffuseColor, m_color, m_fresnelColor, m_packedPosition;
		};
	}

	struct Texture;
	using InitializerListTextures = std::initializer_list<Texture>;
}

constexpr std::string operator""_technique(const char* literal, std::size_t sizeOfString) noexcept
{
	return { literal, sizeOfString };
}

constexpr std::string operator""_fx(const char* literal, std::size_t sizeOfString) noexcept
{
	return { literal, sizeOfString };
}

constexpr std::string operator""_vertexShader(const char* literal, std::size_t sizeOfString) noexcept
{
	return { literal, sizeOfString };
}

constexpr std::string operator""_pixelShader(const char* literal, std::size_t sizeOfString) noexcept
{
	return { literal, sizeOfString };
}

constexpr std::string operator""_constantBuffer(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

constexpr std::string operator""_shaderResource(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

constexpr std::string operator""_mesh(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}
