// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXTex.h>

// Monad
#include "Threads/ThreadPool.h"
#include "Renderer/__MonadRenderer.h"

namespace Monad::Print
{
	/// <summary>
	/// Task that prints the current sketch using a thread pool,
	/// without blocking the window or rendering thread.
	/// </summary>
	struct TaskTexturePrinter final : Threads::ThreadPool::ITask
	{
		TaskTexturePrinter(Renderer::PtrResource rtResource) noexcept;
		void Run() final;

	private:
		Renderer::PtrResource m_rtResource;
	};
}
