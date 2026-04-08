// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

namespace Monad::Kernel
{
	/// <summary>
	/// Dummy parameter type.
	///
	/// Used to explicitly differentiate constructors when the default constructor
	/// is not allowed to throw exceptions or must be avoided.
	/// </summary>
	struct DummyParameter final
	{};
}
