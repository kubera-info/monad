// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <unordered_map>
#include <string>

// Monad
#include "Renderer/__MonadRenderer.h"
#include "Renderer/InputLayout.h"
#include "Kernel/Templates.h"

namespace Monad::Fonts
{
	struct Font;
	using MapFonts = Kernel::UnorderedMapString<Font>;

	extern const std::string
		FONT_CENTURY_GOTHIC,
		FONT_COURIER_NEW,
		FONT_TIMES_NEW_ROMAN;

	constexpr wchar_t
		CHAR_NUL = 0,
		CHAR_LF = L'\n',
		CHAR_SPC = 32,
		CHAR_HORIZONTAL_ELLIPSIS = 0x2026,
		CHAR_HASH = L'#',
		CHAR_BACKSLASH = L'\\';

	enum FONT_STYLES : size_t
	{
		NORMAL,
		BOLD,
		ITALIC,
		BOLD_ITALIC,
		FONT_STYLE_SIZE
	};

	constexpr auto T1_FONT = Monad::Renderer::Registers::INPUT_RESOURCE::T1;
}