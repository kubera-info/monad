// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __LOOKUP_HLSLI__
#define __LOOKUP_HLSLI__

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "params.hlsli"

static float4 GetColorFromTexel(
    const float2 textureCoord
)
{
    float3 color = 0.0;
    for (uint i = 0u; i < g_poissonCount32; ++i)
        color += g_texPalette.Sample(
            g_samplerPoint, 
            g_texArt.Sample(
                g_samplerPoint, 
                textureCoord + g_PoissonTable32[i].xy
            ).x
        ).rgb;
    color /= g_poissonCount32;
    return float4(color, 1.0);
}

static float4 GetColorFromTexel2(
    const float2 textureCoord
)
{
    float3 color = 0.0;
    for (uint i = 0u; i < g_poissonCount32; ++i)
    {
        const float x = g_texArt.Sample(
            g_samplerPoint, 
            textureCoord + g_PoissonTable32[i].xy).x;
        if (x > 0.0)
            color += g_texPalette.Sample(g_samplerPoint, x).rgb;
    }
    color /= g_poissonCount32;
    return float4(color, 1.0);
}
#endif