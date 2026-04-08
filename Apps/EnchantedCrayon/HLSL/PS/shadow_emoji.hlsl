// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/params.hlsli"
#include "../Include/output.hlsli"
#include "../Include/font_vert.hlsli"

Texture2D g_texFont : register(t1);

float main(
    VS_OUTPUT_FONT_SHADOW input
) : SV_DEPTH
{
    const float color = g_texFont.Sample(g_samplerPoint, input.m_texture).a;
    clip(color - .075);
    return input.m_position.z;
}