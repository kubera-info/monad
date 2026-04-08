// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "cForecolor.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

float4 main(
    VS_OUTPUT_COLOR input
) : SV_TARGET
{
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    const float3 ambient = CB::g_forecolor.m_forecolor * g_ambientpower;
    return ConvertToOutput(float4(input.m_color * shadow * .375 + ambient, 1.0));
}