// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

/*#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include "FVF.h"
#include "Math.h"
#include "Wallpaper.h"

MeshCreatorWallpaper::MeshCreatorWallpaper()
{
	VertexType a, b, c, d;
	a.m_position.x = -1.0f * -1.0f;
	a.m_position.y = -1.0f;
	a.m_position.z = 0.0f;
	a.m_tu = 0.0f;
	a.m_tv = 0.0f;

	b.m_position.x = 1.0f * -1.0f;
	b.m_position.y = -1.0f;
	b.m_position.z = 0.0f;
	b.m_tu = 1.0f;
	b.m_tv = 0.0f;

	c.m_position.x = 1.0f * -1.0f;
	c.m_position.y = 1.0f;
	c.m_position.z = 0.0f;
	c.m_tu = 1.0f;
	c.m_tv = 1.0f;

	d.m_position.x = -1.0f * -1.0f;
	d.m_position.y = 1.0f;
	d.m_position.z = 0.0f;
	d.m_tu = 0.0f;
	d.m_tv = 1.0f;

	AddQuad(a, b, c, d);

	CommitMesh();
	SaveMesh(L"wallpaper.x");
}*/