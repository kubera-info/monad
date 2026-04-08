// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Renderer/First.h"
#include "Renderer/Shader.h"
#include "Renderer/ShaderMatrix.h"

namespace Monad::GUI
{
	/// <summary>
	/// Monad encapsulating background rendering logic.
	/// </summary>
	struct Background : 
		Renderer::CalculatePosition<
		Renderer::ShaderTextured<
		Renderer::ShaderNoLight>>
	{
		Background();
	};
}
