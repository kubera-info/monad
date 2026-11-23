// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#ifndef _MONAD_FVF_H_
#define _MONAD_FVF_H_

#include <d3d9.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "../../../MonadEngine/C++/Kernel/__MonadKernel.h"

using namespace DirectX;
using namespace Monad::Kernel;

struct VERTEXPLAIN;

struct VERTEXPANE final
{
	XMFLOAT3 m_position;
	FLOAT    m_tu, m_tv;

	const static D3DVERTEXELEMENT9 Decl[3];
};
struct VERTEXTEX final
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_normal;
	FLOAT    m_tu, m_tv;

	VERTEXTEX() = default;
	VERTEXTEX(const VERTEXPLAIN& Vert);
	VERTEXTEX& operator=(const VERTEXPLAIN& Vert);

	const static D3DVERTEXELEMENT9 Decl[4];
};
struct VERTEXCOL final
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_color;

	const static D3DVERTEXELEMENT9 Decl[3];
};

struct VERTEXPLAIN final
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_normal;

	VERTEXPLAIN() noexcept = default;
	VERTEXPLAIN(const VERTEXPLAIN&) noexcept = default;
	VERTEXPLAIN(VERTEXPLAIN&&) noexcept = default;
	VERTEXPLAIN& operator=(const VERTEXPLAIN&) noexcept = default;
	VERTEXPLAIN& operator=(VERTEXPLAIN&&) noexcept = default;
	VERTEXPLAIN(const VERTEXTEX& Vert);
	VERTEXPLAIN& operator=(const VERTEXTEX& Vert);

	const static D3DVERTEXELEMENT9 Decl[3];
};

struct Declarations
{
	using Decl = Microsoft::WRL::ComPtr<IDirect3DVertexDeclaration9>;

	Declarations();
	void RegisterDecl(const D3DVERTEXELEMENT9 pElem[], Decl& decl);

	Decl	m_pPlainDecl;
	Decl	m_pTexDecl;
	Decl	m_pColDecl;
	Decl	m_pPaneDecl;
};

#endif