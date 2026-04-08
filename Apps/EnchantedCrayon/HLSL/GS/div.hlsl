// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/metal_pipe.hlsli"

void Subdivide(VS_INPUT inVerts[3], out VS_INPUT outVerts[6])
{
    VS_INPUT m[3];
    m[0].m_position = .5f * (inVerts[0].m_position + inVerts[1].m_position);
    m[1].m_position = .5f * (inVerts[1].m_position + inVerts[2].m_position);
    m[2].m_position = .5f * (inVerts[2].m_position + inVerts[0].m_position);
    m[0].m_normal = inVerts[0].m_normal + inVerts[1].m_normal;
    m[1].m_normal = inVerts[1].m_normal + inVerts[2].m_normal;
    m[2].m_normal = inVerts[2].m_normal + inVerts[0].m_normal;

    outVerts[0] = inVerts[0];
    outVerts[1] = m[0];
    outVerts[2] = m[2];
    outVerts[3] = m[1];
    outVerts[4] = inVerts[2];
    outVerts[5] = inVerts[1];
}

void OutputSubdivision(VS_INPUT v[6],
	inout TriangleStream<VS_OUTPUT_METAL_PIPE> triStream)
{
    VS_OUTPUT_METAL_PIPE gout[6];

	[unroll]
    for (uint i = 0; i < 6; ++i)
        gout[i] = metal(v[i]);

    triStream.RestartStrip();

	[unroll]
    for (uint j = 0; j < 5; ++j)
        triStream.Append(gout[j]);
    triStream.Append(gout[1]);
    triStream.Append(gout[5]);
    triStream.Append(gout[3]);
}

[maxvertexcount(8)]
void main(
	triangle VS_INPUT input[3],
	inout TriangleStream<VS_OUTPUT_METAL_PIPE> triStream
)
{
    VS_INPUT v[6];
    Subdivide(input, v);
    OutputSubdivision(v, triStream);
}