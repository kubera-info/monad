// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

float4 main(
    VS_OUTPUT_TEX input
) : SV_TARGET
{
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    return ConvertToOutput(input.m_diffuse * shadow * g_texPalette.Sample(g_samplerAnisotropic, input.m_texture.x));
}