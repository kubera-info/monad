// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "EmojiBuilder.h"

// Monad
#include "__MonadFont.h"
#include "Renderer/__MonadRenderer.h"

namespace Monad
{
	using namespace GUI;

	namespace Fonts
	{
		ShaderEmoji::ShaderEmoji(
			const std::string& emojiName
		) :
			ShaderTextured{
				"emoji"_technique,
				{ Repositories::g_repositoryGeneric->m_shaderResources.LoadEmoji(emojiName) },
				T1_FONT,
				{}
		},
			m_emojiName{ emojiName }
		{}

		void ShaderEmoji::AddEmoji(
			const DirectX::XMFLOAT4& rect
		)
		{
			m_emojiToRender.AddQuad(
				{ { rect.x, rect.y }, { 0.0f, 0.0f } },
				{ { GetRight(rect), rect.y }, { 1.0f, 0.0f } },
				{ { GetRight(rect), GetBottom(rect) }, { 1.0f, 1.0f } },
				{ { rect.x, GetBottom(rect) }, { 0.0f, 1.0f } });
		}

		void ShaderEmoji::OnComputePerShadowFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		)
		{
			__super::OnComputePerShadowFrame(modelSpaceMatrix);

			GetCurrentTexture().SetMe(
				GetCurrentConfig(),
				c_startTexSlot
			);
		}

		void ShaderEmoji::Commit()
		{
			m_mesh = m_emojiToRender.Commit();
			m_emojiToRender.ClearAndShrink();
		}
	}
}