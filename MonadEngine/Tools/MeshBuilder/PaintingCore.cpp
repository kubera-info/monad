// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma message ("In the engine is a newer version, better for some scenarios: MonadGUIFrame.h")

#define _USE_MATH_DEFINES
#include "FVF.h"
#include "../../C++/Math/Math.h"
#include "PaintingCore.h"

constexpr float scale = 0.065f;
constexpr float biggest = 1.0f;
constexpr float biggesty = 1.0f;
constexpr float lerpStep = 0.5f;

/*void Lerp(VERTEXPLAIN& pVert)
{
	pVert.m_position.x += 1.0f;
	pVert.m_position.x *= 0.5f;
	pVert.m_position.y += 1.0f;
	pVert.m_position.y *= 0.5f;
}*/
void MeshCreatorPainting::AddTriangleMetallic(VERTEXPLAIN a, VERTEXPLAIN b, VERTEXPLAIN c, float norm)
{
	const XMFLOAT3 CanonicalNormal(1.0f, 0.0f, 0.0f);
	//Lerp(a);
	//Lerp(b);
	//Lerp(c);
	a.m_normal = b.m_normal = c.m_normal = { norm, 0.0f, 0.0f };
	AddTriangle(a, b, c);
}

void MeshCreatorPainting::Up(const VERTEXPLAIN& leftUpper, const VERTEXPLAIN& leftLower, const VERTEXPLAIN& rightUpper, const VERTEXPLAIN& rightLower)
{
	for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += lerpStep)
	{
		VERTEXPLAIN LeftUpper(leftUpper), LeftLower(leftLower), RightUpper(rightUpper), RightLower(rightLower);
		LeftUpper.m_position.x = std::lerp(leftUpper.m_position.x, rightUpper.m_position.x, lerpVert);
		RightUpper.m_position.x = std::lerp(leftUpper.m_position.x, rightUpper.m_position.x, lerpVert + lerpStep);
		LeftLower.m_position.x = std::lerp(leftLower.m_position.x, rightLower.m_position.x, lerpVert);
		RightLower.m_position.x = std::lerp(leftLower.m_position.x, rightLower.m_position.x, lerpVert + lerpStep);
		AddTriangleMetallic(LeftUpper, LeftLower, RightUpper, 1.0f);
		AddTriangleMetallic(LeftLower, RightLower, RightUpper, 1.0f);
	}
}

void MeshCreatorPainting::Down(const VERTEXPLAIN& leftUpper2, const VERTEXPLAIN& rightUpper2, const VERTEXPLAIN& leftLower2, const VERTEXPLAIN& rightLower2)
{
	for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += lerpStep)
	{
		VERTEXPLAIN LeftUpper2(leftUpper2), RightUpper2(rightUpper2), LeftLower2(leftLower2), RightLower2(rightLower2);
		LeftUpper2.m_position.x = std::lerp(leftUpper2.m_position.x, rightUpper2.m_position.x, lerpVert);
		RightUpper2.m_position.x = std::lerp(leftUpper2.m_position.x, rightUpper2.m_position.x, lerpVert + lerpStep);
		LeftLower2.m_position.x = std::lerp(leftLower2.m_position.x, rightLower2.m_position.x, lerpVert);
		RightLower2.m_position.x = std::lerp(leftLower2.m_position.x, rightLower2.m_position.x, lerpVert + lerpStep);
		AddTriangleMetallic(LeftUpper2, RightUpper2, LeftLower2, .5f);
		AddTriangleMetallic(LeftLower2, RightUpper2, RightLower2, .5f);
	}
}

void MeshCreatorPainting::Left(const VERTEXPLAIN& leftUpper, const VERTEXPLAIN& leftLower2, const VERTEXPLAIN& leftLower, const VERTEXPLAIN& leftUpper2)
{
	for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += lerpStep)
	{
		VERTEXPLAIN LeftUpper(leftUpper), LeftLower2(leftLower2), LeftLower(leftLower), LeftUpper2(leftUpper2);
		LeftUpper.m_position.y = std::lerp(leftUpper.m_position.y, leftLower.m_position.y, lerpVert);
		LeftLower.m_position.y = std::lerp(leftUpper.m_position.y, leftLower.m_position.y, lerpVert + lerpStep);
		LeftUpper2.m_position.y = std::lerp(leftUpper2.m_position.y, leftLower2.m_position.y, lerpVert);
		LeftLower2.m_position.y = std::lerp(leftUpper2.m_position.y, leftLower2.m_position.y, lerpVert + lerpStep);
		AddTriangleMetallic(LeftUpper, LeftLower2, LeftLower, .25f);
		AddTriangleMetallic(LeftLower2, LeftUpper, LeftUpper2, .25f);
	}
}

void MeshCreatorPainting::Right(const VERTEXPLAIN& rightUpper, const VERTEXPLAIN& rightLower, const VERTEXPLAIN& rightLower2, const VERTEXPLAIN& rightUpper2)
{
	for (float lerpVert = .0f; lerpVert < 1.0f; lerpVert += lerpStep)
	{
		VERTEXPLAIN RightUpper(rightUpper), RightLower(rightLower), RightLower2(rightLower2), RightUpper2(rightUpper2);
		RightUpper.m_position.y = std::lerp(RightUpper.m_position.y, rightLower.m_position.y, lerpVert);
		RightLower.m_position.y = std::lerp(RightLower.m_position.y, rightLower.m_position.y, lerpVert + lerpStep);
		RightUpper2.m_position.y = std::lerp(RightUpper2.m_position.y, rightLower2.m_position.y, lerpVert);
		RightLower2.m_position.y = std::lerp(RightLower2.m_position.y, rightLower2.m_position.y, lerpVert + lerpStep);
		AddTriangleMetallic(RightUpper, RightLower, RightLower2, 0.125f);
		AddTriangleMetallic(RightLower2, RightUpper2, RightUpper, 0.125f);
	}
}

MeshCreatorPainting::MeshCreatorPainting()
{
	VERTEXPLAIN LeftUpper, RightUpper;
	VERTEXPLAIN RightLower, LeftLower;
	VERTEXPLAIN LeftUpper2, RightUpper2;
	VERTEXPLAIN RightLower2, LeftLower2;
	const Monad::Math::Ellipse El(8, scale, scale / 5, XM_PI);

	for (float iR = 0; El.GetSidesCount() > iR; iR++)
	{
		float const move = scale * iR / (El.GetSidesCount() * 2);
		float const move2 = scale * (iR + 1) / (El.GetSidesCount() * 2);

		LeftUpper.m_position.z = RightUpper.m_position.z = -El[(DWORD)iR].y;
		LeftLower.m_position.z = RightLower.m_position.z = -El[(DWORD)iR + 1].y;

		LeftUpper.m_position.x = -biggest + move;
		RightUpper.m_position.x = -LeftUpper.m_position.x;
		LeftUpper.m_position.y = RightUpper.m_position.y = biggesty - move;

		LeftLower.m_position.x = -biggest + move2;
		RightLower.m_position.x = -LeftLower.m_position.x;
		LeftLower.m_position.y = RightLower.m_position.y = biggesty - move2;

		Up(LeftUpper, LeftLower, RightUpper, RightLower);

		LeftUpper2 = LeftUpper; RightUpper2 = RightUpper;
		RightLower2 = RightLower; LeftLower2 = LeftLower;

		LeftUpper2.m_position.y = RightUpper2.m_position.y = -RightUpper.m_position.y;
		LeftLower2.m_position.y = RightLower2.m_position.y = -RightLower.m_position.y;

		Down(LeftUpper2, RightUpper2, LeftLower2, RightLower2);

		Left(LeftUpper, LeftLower2, LeftLower, LeftUpper2);

		Right(RightUpper, RightLower, RightLower2, RightUpper2);
	}
	CommitMesh();
	NormalizeMesh();
	SaveMesh(L"dialog_metallic.x");
}
