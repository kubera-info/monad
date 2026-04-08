// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "params.hlsli"
#include "font_vert.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

Texture2D g_texFont : register(t1);

float4 main(
	VS_OUTPUT_EMOJI input
) : SV_TARGET
{
	const float4 output = g_texFont.Sample(g_samplerAnisotropic, input.m_texture);
	if (0.0f == output.a)
		clip(-1);
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    return ConvertToOutput(float4(output.rgb * (shadow * g_shadowpower + g_ambientpower), output.a));
}