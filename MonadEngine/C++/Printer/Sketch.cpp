// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Sketch.h"

// Monad
#include "Files/Temporary.h"
#include "Kernel/Patterns.h"
#include "Renderer/__MonadRenderer.h"

namespace Monad::Print
{
	ShaderSketchGeneric::ShaderSketchGeneric()
		: ShaderTextured(
			"blank_tex"_technique,
			{ Kernel::GHOST },
			Renderer::Registers::INPUT_RESOURCE::T2,
			"pane"_mesh
		),
		m_me(
			this,
			g_shaderSketchGeneric
		)
	{}

	bool ShaderSketchGeneric::IsSystem() const noexcept
	{
		return true;
	}

	void ShaderSketchGeneric::PrintOnce() noexcept
	{
		m_printOnce = PRINT_STATUSES::START;
	}

	void ShaderSketchGeneric::ReleasePrintingLock() noexcept
	{
		m_printOnce = PRINT_STATUSES::READY;
	}

	bool ShaderSketchGeneric::IsVisible() const noexcept
	{
		return m_printOnce != PRINT_STATUSES::READY;
	}

	ShaderSketchGeneric* g_shaderSketchGeneric = nullptr;
}