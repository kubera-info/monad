// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Templates.h"
#include "ConstantBuffers.h"
#include "First.h"
#include "Mesh.h"

using namespace std::string_literals;

namespace Monad::Renderer
{
	// =========================================================================
	// Rendering stage
	// =========================================================================

	enum class STAGE_MODES
	{
		WORLD,
#if defined MONAD_SHADOW
		SHADOW,
#endif
		CUSTOM,
		ALPHA_BLENDING
	};

	extern STAGE_MODES g_stageOfRendering;

	// =========================================================================
	// Shader base class
	// =========================================================================

	/// <summary>
	/// Base shader class.
	/// Owns a mesh and controls rendering passes.
	/// </summary>
	interface ShaderGeneric abstract : First
	{
		/// <summary>
		/// Constructs shader from a mesh file.
		/// </summary>
		ShaderGeneric(
			const std::string& technique,
			Mesh mesh
		);

		/// <summary>
		/// Called by renderer each frame.
		/// Dispatches correct rendering path.
		/// </summary>
		void OnFrameRender(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;

		/// <summary>
		/// Shadow-pass rendering.
		/// </summary>
		virtual void OnComputePerShadowFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		);

		/// <summary>
		/// World-pass rendering.
		/// </summary>
		virtual void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		);

		/// <summary>
		/// Returns currently active shader configuration.
		/// </summary>
		ShaderConfigGeneric* GetCurrentConfig() const noexcept;

	protected:
		Mesh m_mesh;

	private:
		ShaderConfigGeneric* m_shadersWorld = nullptr;

#if defined MONAD_SHADOW
		OptionalShaderConfig m_shadersShadow;
#endif
	};

	// =========================================================================
	// Global constant buffers
	// =========================================================================

	/// <summary>
	/// Global HDR constant buffer.
	/// </summary>
	struct HDRGeneric final : CB::HDRInstance
	{
		HDRGeneric(
			const std::string& counter,
			const BUFFER_TYPE& cbType
		) noexcept;

	private:
		const Kernel::Me<HDRGeneric> c_me;
	};

	extern HDRGeneric* g_dataHDRGeneric;

	/// <summary>
	/// Global morphing matrix used for animated transforms.
	/// </summary>
	struct MorphingMatrix final : DirectX::XMFLOAT4X4
	{
		MorphingMatrix() noexcept;

		const Kernel::Me<DirectX::XMFLOAT4X4> c_me;
	};

	extern DirectX::XMFLOAT4X4* g_dataMorphingMatrix;

	/// <summary>
	/// Global light position buffer.
	/// </summary>
	struct LightPosGeneric final : CB::LightPosInstance
	{
		LightPosGeneric(
			const std::string& counter,
			DirectX::XMFLOAT4 lightPosition
		) noexcept;

	private:
		const Kernel::Me<LightPosGeneric> c_me;
	};

	extern LightPosGeneric* g_dataLightPosMatrixGeneric;
}
