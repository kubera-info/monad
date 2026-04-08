// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Patterns.h"
#include "Shader0.h"
#include "Tex.h"
#include "Repository/Repo.h"

namespace Monad::Renderer
{
	template<typename B>
	interface ShaderTextured abstract : B
	{
	protected:
		/// <summary>
		/// This constructor loads textures, after they are inavailable, before loading process
		/// </summary>
		/// <param name="technique">Shaders</param>
		/// <param name="textureFilenames">Vector of textures ids</param>
		/// <param name="startTexSlot">Texture's slot</param>
		/// <param name="mesh">Model</param>
		ShaderTextured(
			const std::string & technique,
			const Kernel::VectorString & textureFilenames,
			Registers::INPUT_RESOURCE startTexSlot,
			Mesh mesh
		) :
			B{
				technique,
				mesh
		},
				c_startTexSlot{ startTexSlot },
			 m_textures{ std::from_range, textureFilenames }
		{}

		/// <summary>
		/// Constructs shader with textures already loaded,
		/// for example from repository,
		/// after they are available.
		/// </summary>
		/// <param name="technique">Shaders</param>
		/// <param name="textures">Container of textures or initializer_list</param>
		/// <param name="startTexSlot">Texture's slot</param>
		/// <param name="mesh">Model</param>
		ShaderTextured(
			const std::string & technique,
			InitializerListTextures textures,
			Registers::INPUT_RESOURCE startTexSlot,
			Mesh mesh
		) :
			B{
				technique,
				mesh
		},
				c_startTexSlot{ startTexSlot },
				m_textures{ textures }
		{}

		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4 & modelSpaceMatrix
		) override
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			if (!m_textures.empty())
				GetCurrentTexture().SetMe(this->GetCurrentConfig(), c_startTexSlot);
		}
		virtual size_t GetCurrentTextureID() const noexcept { return 0; }
		Texture & GetCurrentTexture() noexcept { return m_textures[GetCurrentTextureID()]; }

		std::vector<Texture> m_textures;
		const Registers::INPUT_RESOURCE c_startTexSlot;
	};
}