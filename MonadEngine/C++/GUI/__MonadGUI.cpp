// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadGUI.h"
// Monad
#include "Fonts/__MonadFont.h"
#include "Audio/__MonadAudio.h"

namespace Monad::GUI
{
	using namespace Fonts;

	const std::string
		JINGLE_CLEAR1{ "clear1"_cue },
		JINGLE_CLEAR2{ "clear2"_cue },
		JINGLE_EXIT{ "exit1"_cue },
		JINGLE_NO{ "no"_cue },
		JINGLE_VOPROS{ "vopros"_cue },
		JINGLE_FOCUS{ "focus"_cue },
		JINGLE_PRINT{ "print1"_cue };

	LPCWSTR RemoveEmojiIcon(
		const I18N::I18NString& title
	) noexcept
	{
		assert(!title.GetVal().empty());
		const auto buf = title.ToLPCWSTR();

		if (CHAR_HASH == *buf)
			for (size_t iChar = 1; ; ++iChar)
				if (buf[iChar] == CHAR_HASH) // The second hash marker was found, signifying the end of the emoji icon data.
					return buf + iChar + 1;
				else if (!buf[iChar]) // Indicates that the end of the input string has been reached and returns an empty string.
					return buf + iChar;

		return buf;
	}
}
