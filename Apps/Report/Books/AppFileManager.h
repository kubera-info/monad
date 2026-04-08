// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Files/Folder.h"
#include "Files/FileManager.h"

namespace Monad::App
{
	/// <summary>
	/// Enumeration describing all possible states
	/// of the file manager state machine.
	/// </summary>
	enum FILE_MAN_STATES : size_t
	{
		LOAD_BOOT_DX,
		RUN_BOOT_DX,
		STOP_BOOT_DX,
	};

	/// <summary>
	/// Performs scanning and loading of application assets.
	/// Responsible for discovering files, scheduling loads,
	/// and managing resource loading states.
	/// </summary>
	struct TaskFileSearcher final : Files::TaskFileSearcherGeneric
	{
		TaskFileSearcher();

	private:
		/// <summary>
		/// Executes file scanning based on the current state.
		/// </summary>
		void Run() final;

		/// <summary>
		/// Handles transitions between file-loading states.
		/// </summary>
		void OnStateChange() noexcept;

		/// <summary>
		/// Calculates the number of shader resource views
		/// required for all loaded assets.
		/// </summary>
		uint32_t SetShaderResourceViewsCount() final;
	};
}