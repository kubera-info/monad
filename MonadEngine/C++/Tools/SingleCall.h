// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <atomic>

namespace Monad::Tools
{
	/// <summary>
	/// A lightweight one-time execution guard.
	///
	/// Internally based on std::atomic_flag, providing a lock-free mechanism
	/// to ensure that a specific action is executed only once.
	///
	/// Typical use cases:
	///  - lazy initialization
	///  - one-time setup logic
	///  - guarding code that must not run more than once
	///  - thread-safe "already executed" marker
	///
	/// The class intentionally exposes no additional API — it relies on
	/// atomic_flag semantics and its inherited operations.
	/// </summary>
	struct SingleCall final : std::atomic_flag
	{
		/// <summary>
		/// Constructor.
		///
		/// If <paramref name="called"/> is true, the internal atomic flag
		/// is immediately set, meaning the guarded operation is considered
		/// already executed.
		///
		/// If false (default), the flag starts cleared and can be set later
		/// via test_and_set().
		///
		/// This allows:
		///  - manual control of initial state
		///  - safe use in static / global objects
		///  - zero-cost initialization
		/// </summary>
		/// <param name="called">
		/// Initial state of the guard:
		/// true  → already executed  
		/// false → not executed yet
		/// </param>
		SingleCall(
			const bool called = false
		) noexcept;
	};
}