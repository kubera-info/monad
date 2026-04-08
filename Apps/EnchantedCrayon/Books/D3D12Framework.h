//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Renderer/Pipeline.h"
#include "Core/DXSample.h"
#include "Input/XBOXControler.h"
#include "Renderer/Combine.h"
#include "Renderer/Shader0.h"

/// <summary>
/// DXGI and D3D12 implementation
/// </summary>
struct D3D12Core : DXSample
{
	D3D12Core();
	~D3D12Core() override;
	OPER_DEL_NO_DEF_CTOR(D3D12Core);

private:
	void NewFrame();
	/// <summary>
	/// Update frame-based values.
	/// </summary>
	void OnUpdate() final;
	/// <summary>
	/// Render the scene.
	/// </summary>
	void OnRender() final;

public:
	Monad::Renderer::RenderTarget m_screenshotRT;
	const Monad::Input::ThreadXboxController m_xBoxController;

	/// <summary>
	/// Fill the command list with all the render commands and dependent state.
	/// </summary>
	void PopulateCommandLists();
};