// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Tools/SingleCall.h"

namespace Monad::Tools
{
	/// <summary>
	/// Constructs a SingleCall guard.
	///
	/// If <paramref name="called"/> is true, the internal flag is immediately set,
	/// which means the guarded operation is considered already executed.
	///
	/// This allows:
	///  - conditional one-time execution
	///  - external control over initial state
	///  - safe use in constructors and static objects
	///
	/// No synchronization primitives are required here because:
	/// - test_and_set() is atomic
	/// - the class is designed for very small critical sections
	/// </summary>
	SingleCall::SingleCall(
		const bool called
	) noexcept
	{
		if (called)
			test_and_set();
	}
}