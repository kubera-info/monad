// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __LOOKUP_HLSLI__
#define __LOOKUP_HLSLI__

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "../../../../MonadEngine/HLSL/color_space.hlsli"
#include "params.hlsli"

static float4 GetColorFromTexel(
    const float2 textureCoord
)
{
    float3 color = 0.0;
    for (uint i = 0u; i < g_poissonCount16; ++i)
        color += g_texPalette.SampleLevel(
            g_samplerPoint, 
            g_texArt.SampleLevel(
                g_samplerPoint, 
                textureCoord + g_PoissonTable16[i].xy,
                0
            ).x,
        0
        ).rgb;
    color /= g_poissonCount16;
    return float4(color, 1.0);
}

static float4 GetColorFromTexel2(
    const float2 textureCoord
)
{
    float3 color = 0.0;
    for (uint i = 0u; i < g_poissonCount16; ++i)
    {
        const float x = g_texArt.SampleLevel(
            g_samplerPoint, 
            textureCoord + g_PoissonTable16[i].xy,
        0
        ).x;
        if (x > 0.0)
        {
            color += g_texPalette.SampleLevel(
            g_samplerPoint, 
            x, 
            0
            ).rgb;
        }
    }
    color /= g_poissonCount16;
    return float4(LinearToSRGB(color), 1.0);
}
#endif