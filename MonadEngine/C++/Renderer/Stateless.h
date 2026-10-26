// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <D3D12.h>

// Monad
#include "CommandList.h"
#include "__MonadRenderer.h"

struct DXSample;

namespace Monad::Renderer
{
	struct Stateless
	{
		Stateless();
		Stateless(const Stateless&) = delete;
		Stateless& operator=(const Stateless&) = delete;
		Stateless(Stateless&&) = delete;
		Stateless& operator=(Stateless&&) = delete;
		void Commit();

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

	public:
		GraphicsCommandList1 m_commandList;
	};
}