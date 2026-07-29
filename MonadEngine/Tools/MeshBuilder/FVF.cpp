// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "FVF.h"

extern Microsoft::WRL::ComPtr<IDirect3DDevice9> pD3DDevice;

VERTEXTEX::VERTEXTEX(const VERTEXPLAIN& Vert) :
	m_position(Vert.m_position),
	m_normal(Vert.m_normal),
	m_tu(0.0f),
	m_tv(0.0f)
{
}

VERTEXTEX& VERTEXTEX::operator=(const VERTEXPLAIN& Vert)
{
	m_position = Vert.m_position;
	m_normal = Vert.m_normal;
	m_tu = 0.0f;
	m_tv = 0.0f;

	return *this;
}

VERTEXPLAIN::VERTEXPLAIN(const VERTEXTEX& Vert)
{
	m_position = Vert.m_position;
	m_normal = Vert.m_normal;
}
VERTEXPLAIN& VERTEXPLAIN::operator=(const VERTEXTEX& Vert)
{
	m_position = Vert.m_position;
	m_normal = Vert.m_normal;

	return *this;
}

const D3DVERTEXELEMENT9 VERTEXPANE::Decl[3] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

const D3DVERTEXELEMENT9 VERTEXTEX::Decl[4] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

const D3DVERTEXELEMENT9 VERTEXCOL::Decl[3] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	D3DDECL_END()
};

const D3DVERTEXELEMENT9 VERTEXPLAIN::Decl[3] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
	D3DDECL_END()
};

Declarations::Declarations()
{
	RegisterDecl(VERTEXPLAIN::Decl, m_pPlainDecl);
	RegisterDecl(VERTEXTEX::Decl, m_pTexDecl);
	RegisterDecl(VERTEXCOL::Decl, m_pColDecl);
	RegisterDecl(VERTEXPANE::Decl, m_pPaneDecl);
}
void Declarations::RegisterDecl(const D3DVERTEXELEMENT9 pElem[], Decl& decl)
{
	pD3DDevice->CreateVertexDeclaration(pElem, &decl);
}