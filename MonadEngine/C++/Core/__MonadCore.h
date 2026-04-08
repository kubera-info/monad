// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Renderer/__MonadRenderer.h"

/// <summary>
/// Represents a placeholder (dummy) shader resource view.
/// Used as a safe default to ensure valid binding when no real resource is available.
/// </summary>
constexpr uint32_t SRV_GHOST = 1;

/// <summary>
/// Stores counters related to rendering resources.
/// </summary>
struct Counts
{
	/// <summary>
	/// Number of CBV/SRV/UAV descriptors explicitly created by the application.
	/// Constant buffers are not included here.
	/// </summary>
	const uint32_t c_bareCbvSrvCount;

	/// <summary>
	/// Number of shader resources discovered and loaded from asset files
	/// by the File Manager.
	/// </summary>
	const uint32_t c_foundSrvFilesCount;
};

/// <summary>
/// Shadow map resource slot.
/// Always available regardless of shadow rendering state
/// and must never be released.
/// </summary>
constexpr auto T0_SHADOW_MAP = Monad::Renderer::Registers::INPUT_RESOURCE::T0;

/// <summary>
/// Texture slot used for artwork (coloring pages, illustrations, etc.).
/// </summary>
constexpr auto T2_ART = Monad::Renderer::Registers::INPUT_RESOURCE::T2;
