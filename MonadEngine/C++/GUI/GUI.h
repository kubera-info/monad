// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Audio/Jingle.h"
#include "Pages/Page.h"
#include "GUIMath.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderMatrix.h"

namespace Monad::GUI
{
#pragma region Interfaces
	/// <summary>
	/// Interface defining click-handling behavior for UI elements.
	/// Supports input from various pointing devices, including the Xbox controller.
	/// </summary>
	interface Clickable abstract : Kernel::EventPointerDownStub
	{
		Clickable(
			const std::string & pageNo,
			const Audio::RandomJingle & clicksSounds
		);

	protected:
		virtual void OnClick() = 0;
		virtual bool OnRegion() const noexcept = 0;
		inline bool IsInRegion() const noexcept { return c_pageNo == Pages::g_currentPage && OnRegion(); }
		inline void PlayClickJingle() const { c_clicksSounds.PlayJingle(); }

	private:
		bool OnPointerDownStub() final;

		/// <summary>
		/// Plays randomly selected jingles to enhance variability in audio feedback.
		/// </summary>
		const Audio::RandomJingle c_clicksSounds;
		const std::string c_pageNo;
	};
#pragma endregion Interfaces

#pragma region Primitives
	/// <summary>
	/// Abstract base class for renderable UI elements.
	/// </summary>
	interface Pane abstract :
	Renderer::CalculatePosition<
		Renderer::ShaderTextured<
		Renderer::Shader>>
	{
		Pane(
			const DirectX::XMFLOAT4& positionOfCenter,
			const std::string& technique,
			const Kernel::VectorString& textureFilenames,
			const std::string& meshFilename
		);
		const DirectX::XMFLOAT2 GetPos() const noexcept;

	protected:
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;

		const DirectX::XMFLOAT4 m_positionOfCenter;
	};
#pragma endregion Primitives
}
