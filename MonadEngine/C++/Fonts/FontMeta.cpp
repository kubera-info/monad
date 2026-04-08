// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "FontMeta.h"

using namespace std;

struct DXSample;

namespace Monad
{
	using namespace Kernel;
	using namespace Renderer;

	namespace Fonts
	{
		namespace
		{
			DirectX::XMFLOAT2 g_carriagePos;
			float g_carriageHorizontalPos;
		}

#pragma region Font
		Font::Char::Char(
			const DirectX::XMFLOAT4& chunk,
			const DirectX::XMFLOAT2 offset,
			float advance
		) noexcept :
			c_chunk{ chunk },
			c_offset{ offset },
			c_advance{ advance }
		{}

		float Font::Char::CalculateDistance(
			wchar_t char2
		) const
		{
			const auto kerning = m_kernings.find(char2);
			return c_advance + (m_kernings.cend() != kerning ? kerning->second : 0.0f);
		}
	
		void Font::SetFontScale(
			float lineHeight
		) noexcept
		{
			m_lineHeight = lineHeight;
		}
#pragma endregion
	}
}