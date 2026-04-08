// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../../../../MonadEngine/HLSL/struct.hlsli"
#include "output.hlsli"
#include "params.hlsli"
#include "../../../../MonadEngine/HLSL/cHDRSettings.hlsli"

float4 main(
    VS_OUTPUT_TEX input
) : SV_TARGET
{
	const float shadow = CalculateShadowBlur(input.m_projectedCoord);
    const float4 texel = SRGBAToLinear(g_texArt.Sample(g_samplerAnisotropic, input.m_texture));
    return ConvertToOutput((shadow * g_shadowpower + g_ambientpower) * texel);
}