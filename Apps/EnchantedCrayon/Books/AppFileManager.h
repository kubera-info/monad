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
		RUN_BOOT,
		STOP_BOOT,

		LOAD_BOOT_DX,
		RUN_BOOT_DX,
		STOP_BOOT_DX,

		LOAD_ALL,
		RUN_ALL,
		STOP_ALL,

		CHECK_ALL,
		MERGE_ALL,

		LOAD_PROFILE,
		RUN_PROFILE,
		STOP_PROFILE,

		LOAD_I18N_DESC,
		RUN_I18N_DESC
	};

	/// <summary>
	/// Performs scanning and loading of application assets.
	/// Responsible for discovering files, scheduling loads,
	/// and managing resource loading states.
	/// </summary>
	struct TaskFileSearcher final : Files::TaskFileSearcherGeneric
	{
		TaskFileSearcher();
		/// <summary>
		/// Configures all runtime-dependent asset paths
		/// (e.g. quality level, localization).
		/// </summary>
		void SetPaths();

	private:
		/// <summary>
		/// Executes file scanning based on the current state.
		/// </summary>
		void Run() final;
		/// <summary>
		/// Handles transitions between file-loading states.
		/// </summary>
		void OnStateChange() noexcept final;
		/// <summary>
		/// Counts future shader resource views and arts
		/// </summary>
		uint32_t SetShaderResourceViewsCount() final;
	};
}