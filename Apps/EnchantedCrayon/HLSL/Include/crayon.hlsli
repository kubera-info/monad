// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

float4 main(
    VS_OUTPUT_COLOR input
) : SV_TARGET
{
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    return float4(ConvertToOutput(input.m_color.rgb * shadow), 1.0);
}