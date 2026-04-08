// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "output.hlsli"
#include "lookup.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

float4 main(
	VS_OUTPUT_TEX input
) : SV_TARGET
{
	const float4 texPoint = g_texArt.Sample(g_samplerAnisotropic, input.m_texture);
	if (0.0 == texPoint.a)
		clip(-1);
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    return ConvertToOutput(float4(texPoint.rgb * shadow, texPoint.a));
}