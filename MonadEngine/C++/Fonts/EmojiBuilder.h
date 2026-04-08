// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "GUI/GUIMath.h"
#include "Renderer/MeshCore.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderMatrix.h"

namespace Monad::Fonts
{
	/// <summary>
	/// A monad representing a single type of emoji for dialog boxes
	/// </summary>
	struct ShaderEmoji final : Renderer::ShaderTextured<Renderer::ShaderNoLight>
	{
		ShaderEmoji(
			const std::string& emojiName
		);
		/// <summary>
		/// Creates and registers a dedicated quad for the emoji, with separate vertex data, index data, and rendering resources.
		/// </summary>
		/// <param name="rect">A place in the dialog</param>
		void AddEmoji(
			const DirectX::XMFLOAT4& rect
		);
		void OnComputePerShadowFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) final;
		///<summary>
		/// Generates a renderable mesh representing the emoji.
		/// </summary>
		void Commit();

		const std::string m_emojiName;

	private:
		Renderer::InputLayout::MeshBuilderEmoji m_emojiToRender;
	};
}
