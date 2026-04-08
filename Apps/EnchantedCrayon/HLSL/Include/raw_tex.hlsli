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
	const float shadow = CalculateShadowBlur(input.m_projectedCoord) * g_shadowpower + g_ambientpower;
    return ConvertToOutput(GetColorFromTexel(input.m_texture) * float4(shadow, shadow, shadow, 1.0));
}