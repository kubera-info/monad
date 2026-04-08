// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
// STD
#include <array>
#include <unordered_map>
// Monad
#include "Fonts/__MonadFont.h"

namespace Monad::Fonts
{
	struct Text2Mesh;

	struct StyleInFont
	{
		bool m_italicFound = false, m_boldFound = false;
	};

	/// <summary>
	/// A one dialog font combine
	/// </summary>
	struct Font final
	{
		friend Text2Mesh;

		void SetFontScale(
			float lineHeight
		) noexcept;
		Font& operator=(
			const Font&
			) = delete;
		// One character, metadata
		struct Char final
		{
			friend Text2Mesh;

			Char(
				const DirectX::XMFLOAT4& chunk,
				const DirectX::XMFLOAT2,
				float advance
			) noexcept;
			float CalculateDistance(
				wchar_t char2
			) const;
			using MapKernings = std::unordered_map<wchar_t, const float>;
			MapKernings m_kernings;

		private:
			const DirectX::XMFLOAT4 c_chunk;
			const DirectX::XMFLOAT2 c_offset;
			const float c_advance;
		};
		template<class Self>
		auto& operator()(
			this Self&& self,
			wchar_t char2,
			FONT_STYLES style
			)
		{
			decltype(auto) meta = std::forward<Self>(self).m_metaChars[style];
			auto chr = meta.find(char2);
			if (meta.cend() == chr)
				chr = meta.find(CHAR_SPC);
			return chr->second;
		}

		StyleInFont m_styleInFont;

	private:
		float m_lineHeight;

	public:
		std::array<std::unordered_map<wchar_t, Char>, FONT_STYLE_SIZE> m_metaChars;
	};
}