// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <cassert>
#include <random>
// Monad
#include "Kernel/Del.h"

namespace Monad::Math
{
	/// <summary>
	/// Random number generator utility.
	/// Provides helpers for generating random integer values and signs.
	/// </summary>
	struct Random final
	{
		Random();
		~Random() = default;

		OPER_DEL_NO_DEF_CTOR(Random);

		/// <summary>
		/// Returns a random integer value in the specified range.
		/// </summary>
		/// <typeparam name="I">Integral type.</typeparam>
		/// <param name="minimum">Lower bound (inclusive).</param>
		/// <param name="maximum">Upper bound (inclusive).</param>
		/// <returns>Randomly generated integer.</returns>
		template<typename I>
		inline I GetRandomInt(
			const I minimum,
			const I maximum
		)
		{
			assert(maximum >= minimum && "Invalid random range");
			std::uniform_int_distribution<I> distribution(minimum, maximum);
			return distribution(m_generator);
		}

		/// <summary>
		/// Returns a random integer value in the range [0, maximum].
		/// </summary>
		/// <typeparam name="I">Integral type.</typeparam>
		/// <param name="maximum">Upper bound (inclusive).</param>
		/// <returns>Randomly generated integer.</returns>
		template<typename I>
		inline I GetRandomInt(const I maximum)
		{
			return GetRandomInt<I>(0, maximum);
		}

		/// <summary>
		/// Returns either -1.0f or 1.0f randomly.
		/// </summary>
		/// <returns>Random sign value.</returns>
		float GetFlip();

	private:
		std::random_device m_rd;
		std::mt19937 m_generator;
	};

	/// <summary>
	/// Global random number generator instance.
	/// </summary>
	extern Random g_random;
}
