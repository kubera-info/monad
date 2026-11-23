// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include <d3dx9.h>
#include <WRL.h>
#include "FVF.h"
#include "kredka.h"
#include "MeshCore.h"

extern Microsoft::WRL::ComPtr<IDirect3DDevice9> pD3DDevice;

Kredka::Kredka()
{
	const int cSlices = 16;
	const float cRadius = .0135f;
	const float cSmallRadius = .0015;
	const float cLabelRadius = cRadius * 1.05f;
	const float length = .275f;
	const float cSmallLength = cRadius * 3.0f;
	const float cLabelLength = length * .9f;
	Microsoft::WRL::ComPtr<ID3DXMesh> mesh, pMesh2, pMesh3;

	if (FAILED(D3DXCreateCylinder(
		pD3DDevice.Get(),
		cRadius,
		cRadius,
		length,
		cSlices,
		4,
		&mesh,
		NULL
	)))
	{
		MessageBox(0, L"error", L"error", 0);
		exit(0);
	}

	if (FAILED(D3DXCreateCylinder(
		pD3DDevice.Get(),
		cRadius,
		cSmallRadius,
		cSmallLength,
		cSlices,
		1,
		&pMesh2,
		NULL
	)))
	{
		MessageBox(0, L"error", L"error3", 0);
		exit(0);
	}

	if (FAILED(D3DXCreateCylinder(
		pD3DDevice.Get(),
		cLabelRadius,
		cLabelRadius,
		cLabelLength,
		cSlices,
		2,
		&pMesh3,
		NULL
	)))
	{
		MessageBox(0, L"error", L"error3", 0);
		exit(0);
	}

	struct CCrayon : CMeshBuilderPlain
	{
		VERTEXPLAIN TransformMe(const VERTEXPLAIN& vert)
		{
			VERTEXPLAIN vert2(vert);
			XMVECTOR vv = XMLoadFloat3(&vert.m_position);
			vv = XMVector3Transform(vv, XMLoadFloat4x4(&transform));
			XMStoreFloat3(&vert2.m_position, vv);
			return vert2;
		}
		void SetMatrix(float pMoveX, float pMoveY, float pMoveZ)
		{
			XMStoreFloat4x4(&transform, XMMatrixTranslation(pMoveX, pMoveY, pMoveZ));
		}
		virtual void OnParseTriangle(const VERTEXPLAIN& a, const VERTEXPLAIN& b, const VERTEXPLAIN& c)
		{
			VERTEXPLAIN newA(TransformMe(a)), newB(TransformMe(b)), newC(TransformMe(c));

			AddTriangle2(newA, newB, newC);
		}

		XMFLOAT4X4 transform;
	} crayon, crayonlabel;
	const float smallMid = cSmallLength / 2.0f;
	const float bigMid = cSmallLength + (length / 2.0f);
	crayon.SetMatrix(0.0f, 0.0f, -smallMid);
	crayon.ParseMesh(pMesh2.Get());
	crayon.SetMatrix(0.0f, 0.0f, -bigMid);
	crayon.ParseMesh(mesh.Get());
	crayon.CommitMesh();
	crayon.NormalizeMesh();
	crayon.SaveMesh(L"crayon.x");

	crayonlabel.SetMatrix(0.0f, 0.0f, -bigMid);
	crayonlabel.ParseMesh(pMesh3.Get());
	crayonlabel.CommitMesh();
	crayonlabel.NormalizeMesh();
	crayonlabel.SaveMesh(L"label.x");
}
