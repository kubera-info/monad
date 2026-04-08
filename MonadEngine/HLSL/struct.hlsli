// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __STRUCT_HLSLI__
#define __STRUCT_HLSLI__

#include "poisson.hlsli"

SamplerComparisonState g_samplerCmp : register(s2);
Texture2D g_texShadowMap : register(t0);
static const float g_shadowpower = .8;
static const float g_ambientpower = 1.0 - g_shadowpower;

// Routine to blur monochrome image
static float CalculateShadowBlur(
    float4 position
)
{
    position.xyz /= position.w;
    position.x = +.5 * position.x + .5;
    position.y = -.5 * position.y + .5;
    const float
        epsylon = .000075,
        depth = position.z - epsylon;
    float countShadowPoints = 0u;
#   if defined(NO_SHADOW)
	for (uint i = 0u; i < g_poissonCount4; ++i)
		countShadowPoints += g_texShadowMap.SampleCmpLevelZero(g_samplerCmp, position.xy + g_PoissonTable4[i].xy, depth);
	return countShadowPoints / g_poissonCount4;
#   elif defined(SINGLE_SHADOW)
	for (uint i = 0u; i < g_poissonCount16; ++i)
		countShadowPoints += g_texShadowMap.SampleCmpLevelZero(g_samplerCmp, position.xy + g_PoissonTable16[i].xy, depth);
	return countShadowPoints / g_poissonCount16;
#   else
    for (uint i = 0u; i < g_poissonCount32; ++i)
        countShadowPoints += g_texShadowMap.SampleCmpLevelZero(g_samplerCmp, position.xy + g_PoissonTable32[i].xy, depth);
    return countShadowPoints / g_poissonCount32;
#   endif
}
#endif