// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __METAL_HLSLI__
#define __METAL_HLSLI__

#include "cPipe.hlsli"
#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "../../../../MonadEngine/HLSL/tools.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

float4 main(
	VS_OUTPUT_METAL_PIPE input
) : SV_TARGET
{
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
	const float3 brightness =
		(FromFloat(
		CB::g_pipeColor.m_diffuseColor,
		pow(saturate(dot(input.m_n, input.m_lNew)), 2.2)) +
	FromFloat(
		CB::g_pipeColor.m_color,
		pow(saturate(dot(input.m_h, input.m_n)), 132.0) * 6.0)) * 1.25;
	const float fresnel0 = pow(abs(1 - 4 * input.m_f * input.m_f), 32.0) * .425;
    const float3 fresnel = FromFloat(CB::g_pipeColor.m_fresnelColor, fresnel0);
    return ConvertToOutput(saturate(float4(brightness * shadow + fresnel, 1.0)));
}
#endif