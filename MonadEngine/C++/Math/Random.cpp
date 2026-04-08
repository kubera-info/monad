// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Random.h"

namespace Monad::Math
{
	Random::Random() :
		m_generator{ m_rd() }
	{}

	float Random::GetFlip()
	{
		return static_cast<float>((GetRandomInt(1) << 1) - 1);
	}

	Random g_random;
}