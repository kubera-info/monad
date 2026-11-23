// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#include "MeshCore.h"

struct MeshCreatorPainting : CMeshBuilderPlain
{
	MeshCreatorPainting();
	void Up(const VERTEXPLAIN& leftUpper, const VERTEXPLAIN& leftLower, const VERTEXPLAIN& rightUpper, const VERTEXPLAIN& rightLower);
	void Down(const VERTEXPLAIN& leftUpper2, const VERTEXPLAIN& rightUpper2, const VERTEXPLAIN& leftLower2, const VERTEXPLAIN& rightLower2);
	void Left(const VERTEXPLAIN& leftUpper, const VERTEXPLAIN& leftLower2, const VERTEXPLAIN& leftLower, const VERTEXPLAIN& leftUpper2);
	void Right(const VERTEXPLAIN& rightUpper, const VERTEXPLAIN& rightLower, const VERTEXPLAIN& rightLower2, const VERTEXPLAIN& rightUpper2);
	void AddTriangleMetallic(VERTEXPLAIN a, VERTEXPLAIN b, VERTEXPLAIN c, float norm);
};