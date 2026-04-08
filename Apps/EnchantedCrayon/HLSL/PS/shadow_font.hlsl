// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../../../../MonadEngine/HLSL/poisson.hlsli"
#include "../Include/params.hlsli"
#include "../Include/output.hlsli"
#include "../Include/font_vert.hlsli"

Texture2D g_texFont : register(t1);

static float CalculateFontBlur(
    float2 font
)
{
    float countShadowPoints = 0;
    for (uint i = 0; i < g_poissonCount16; ++i)
        countShadowPoints += g_texFont.Sample(g_samplerPoint, font).r;
    return countShadowPoints / g_poissonCount16;
}

float main(
    VS_OUTPUT_FONT_SHADOW input
) : SV_DEPTH
{
    const float Color = CalculateFontBlur(input.m_texture);
    clip(Color - .075);
    return input.m_position.z;
}