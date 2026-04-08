// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

namespace Monad::App
{
	/// <summary>
	/// Notifies the system about pointer press or release events
	/// (e.g. mouse button, touch input, controller trigger).
	/// </summary>
	/// <param name="leftButtonDown">
	/// True if the pointer is currently pressed, false if released.
	/// </param>
	void IsMouseDownStub(
		const bool leftButtonDown
	);
}