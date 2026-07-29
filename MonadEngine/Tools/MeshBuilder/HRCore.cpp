// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "MeshCore.h"
#include <cmath>
#include "HRCore.h"

extern Microsoft::WRL::ComPtr<IDirect3DDevice9> pD3DDevice;

void Skew(VERTEXPLAIN& a)
{
	const float fSkewFactor = powf(abs(a.m_position.x), 2.9f);

	a.m_position.z *= (1.0f - fSkewFactor);
	a.m_position.x *= fSkewFactor;
}
MeshHRCreator::MeshHRCreator()
{
	struct CHRCreator : CMeshBuilderPlain
	{
		void AddTriangleMetallic(VERTEXPLAIN a, VERTEXPLAIN b, VERTEXPLAIN c, float norm)
		{
			a.m_normal = b.m_normal = c.m_normal = { norm, 0, 0 };

			AddTriangle(a, b, c);
		}
	} collector;
	struct Metal : CMeshBuilderPlain
	{
		void OnParseTriangle(const VERTEXPLAIN& a, const VERTEXPLAIN& b, const VERTEXPLAIN& c) final
		{
			const XMVECTOR av = XMLoadFloat3(&a.m_position), bv = XMLoadFloat3(&b.m_position), cv = XMLoadFloat3(&c.m_position);
			if (XMVector3Equal(av, bv)
				|| XMVector3Equal(av, cv)
				|| XMVector3Equal(bv, cv))
				return;

			VERTEXPLAIN newA(a), newB(b), newC(c);

			Skew(newA);
			Skew(newB);
			Skew(newC);

			AddTriangle(newA, newB, newC);
		}
	} metal;

	VERTEXPLAIN center, top, /*down,*/ left /*, right*/;
	center.m_normal.x = 1.0f;
	center.m_normal.y = 0.0f;
	center.m_normal.z = 0.0f;
	center.m_position.x = 0.0f;
	center.m_position.y = 0.0f;
	center.m_position.z = 0.005f;

	top = center;
	top.m_position.y = -0.0185f;

	left = center;
	left.m_position.x = 1.0f;

	const float steps = 64.0f;

	std::vector<VERTEXPLAIN> VecOfVert;

	for (float i = 0.0f; i <= steps; i++)
	{
		VERTEXPLAIN Vertex;

		Vertex.m_position.x = left.m_position.x * (i / steps);
		Vertex.m_position.y = top.m_position.y * ((steps - i) / steps);
		Vertex.m_position.z = 0.0f;

		VecOfVert.push_back(Vertex);
	}

	for (int i = 0; i < steps; i++)
	{
		VERTEXPLAIN a, b, c, d;
		a = VecOfVert[i];
		b = VecOfVert[i + 1];
		c = b; c.m_position.y = 0;
		d = a;
		d.m_position.y = 0;
		c.m_position.z = std::lerp(center.m_position.z, 0.0f, float(i + 1) / float(steps));
		d.m_position.z = std::lerp(center.m_position.z, 0.0f, float(i) / float(steps));
		collector.AddTriangleMetallic(b, a, c, 1.0f);
		collector.AddTriangleMetallic(d, c, a, 1.0f);

		a.m_position.x *= -1.0f;
		b.m_position.x *= -1.0f;
		c.m_position.x *= -1.0f;
		d.m_position.x *= -1.0f;
		collector.AddTriangleMetallic(b, c, a, 0.5f);
		collector.AddTriangleMetallic(d, a, c, 0.5f);

		a.m_position.y *= -1.0f;
		b.m_position.y *= -1.0f;
		c.m_position.y *= -1.0f;
		d.m_position.y *= -1.0f;
		collector.AddTriangleMetallic(b, a, c, 0.25f);
		collector.AddTriangleMetallic(d, c, a, 0.25f);

		a.m_position.x *= -1.0f;
		b.m_position.x *= -1.0f;
		c.m_position.x *= -1.0f;
		d.m_position.x *= -1.0f;
		collector.AddTriangleMetallic(b, c, a, 0.125f);
		collector.AddTriangleMetallic(d, a, c, 0.125f);
	}
	collector.CommitMesh();

	metal.ParseMesh(collector.GetMesh());
	metal.CommitMesh();
	metal.NormalizeMesh();
	struct CHRNromals : CMeshBuilderPlain
	{
		void CalcNrm(VERTEXPLAIN& forNrm)
		{
			if (0.0f == forNrm.m_position.z)
				forNrm.m_normal.z *= 1.125f;
			else
				forNrm.m_normal.z *= 0.425f;
			XMVECTOR Nrm = XMLoadFloat3(&forNrm.m_normal);
			Nrm = XMVector3Normalize(Nrm);
			XMStoreFloat3(&forNrm.m_normal, Nrm);
		}
		void OnParseTriangle(const VERTEXPLAIN& a, const VERTEXPLAIN& b, const VERTEXPLAIN& c) final
		{
			VERTEXPLAIN newA(a), newB(b), newC(c);
			CalcNrm(newA);
			CalcNrm(newB);
			CalcNrm(newC);
			AddTriangle(newA, newB, newC);
		}
	} normals;

	metal.CleanMesh();
	normals.ParseMesh(metal.GetMesh());
	normals.CommitMesh();
	//metal.WeldMesh(.0001, .0001);
	normals.SaveMesh(L"hr.x");
}