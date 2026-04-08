// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <D3D12.h>
// Monad
#include "Kernel/Del.h"
#include "CommandList.h"
#include "__MonadRenderer.h"

struct DXSample;

namespace Monad::Renderer
{
	struct Stateless
	{
		Stateless();
		~Stateless() = default;
		OPER_DEL_NO_DEF_CTOR(Stateless);
		void Commit();

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

	public:
		GraphicsCommandList1 m_commandList;
	};
}