// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/input.hlsli"
#include "../Include/output.hlsli"
#include "../Include/cPipe.hlsli"
#include "../Include/cWorldViewProj.hlsli"
#include "../Include/cLightPos.hlsli"
#include "../Include/cModel.hlsli"
#include "../../../../MonadEngine/HLSL/tools.hlsli"

VS_OUTPUT_COLOR main(
    VS_INPUT input
)
{
    VS_OUTPUT_COLOR output;

    const float3 p = mul(input.m_position, (float4x3) CB::g_model.m_model), // position (view space)
    lNew = normalize(CB::g_lightPos.m_lightPosition - p),
    n = normalize(mul(input.m_normal, (float3x3) CB::g_model.m_model)), // normal (view space)
    v = -normalize(p), // view direction (view space)
    h = normalize(lNew + v); // half vector (view space)

	// m_position (projected)
    output.m_position = mul(input.m_position, CB::g_model.m_modelViewProj);
	// ArrayColors
    const float4 light = lit(dot(n, lNew), dot(n, h), 10.0);
    output.m_color =
		FromFloat(
			CB::g_pipeColor.m_diffuseColor,
			light.y) +
		FromFloat(
			CB::g_pipeColor.m_color,
			light.z);
	// Shadow mapping
    output.m_projectedCoord = mul(input.m_position, CB::g_shadow.m_shadowModelViewProj);

    return output;
}