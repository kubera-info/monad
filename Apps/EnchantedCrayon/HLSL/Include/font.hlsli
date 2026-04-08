// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __FONT_HLSLI__
#define __FONT_HLSLI__

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "font_vert.hlsli"
#include "params.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

Texture2D g_texFont : register(t1);

float4 main(
	VS_OUTPUT_FONT input
) : SV_TARGET
{
	const float output = g_texFont.Sample(g_samplerAnisotropic, input.m_texture).r;
	clip(output - .25);
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    return ConvertToOutput(float4(input.m_color * shadow, output));
}
#endif