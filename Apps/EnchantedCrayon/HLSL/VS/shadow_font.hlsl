// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/cWorldViewProj.hlsli"
#include "../Include/input.hlsli"
#include "../Include/output.hlsli"
#include "../Include/font_vert.hlsli"

VS_OUTPUT_FONT_SHADOW main(
    VS_FONT input
)
{
    VS_OUTPUT_FONT_SHADOW output;

    output.m_position = mul(CalculateVertexPosition(input.m_position), CB::g_shadow.m_shadowModelViewProj);
    output.m_texture = input.m_texture;

    return output;
}