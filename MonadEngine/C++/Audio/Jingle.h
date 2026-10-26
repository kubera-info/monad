// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "__MonadAudio.h"
#include "Persistent.h"
#include "Kernel/Patterns.h"

namespace Monad::Audio
{
	/// <summary>
	/// Structure for playing random jingles to introduce some unpredictability.
	/// </summary>
	struct RandomJingle final : private VectorRandomCue
	{
		using VectorRandomCue::VectorRandomCue;
		/// <summary>
		/// Immediately triggers playback of a jingle, such as a click.
		/// </summary>
		void PlayJingle() const;
	};
}
