// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "input.hlsli"
#include "output.hlsli"
#include "cPipe.hlsli"
#include "cWorldViewProj.hlsli"
#include "cLightPos.hlsli"
#include "cModel.hlsli"
#include "../../../../MonadEngine/HLSL/tools.hlsli"

VS_OUTPUT_METAL_PIPE metal(
	VS_INPUT input
)
{
    VS_OUTPUT_METAL_PIPE output;

    const float3 p = mul(input.m_position, (float4x3) CB::g_model.m_model), // position (view space)
    v = -normalize(p); // view direction (view space)
    output.m_lNew = normalize(CB::g_lightPos.m_lightPosition - p);
    output.m_n = normalize(mul(input.m_normal, (float3x3) CB::g_model.m_model)); // normal (view space)
    output.m_h = normalize(output.m_lNew + v); // half vector (view space)
    output.m_f = .5 - dot(v, output.m_n);

	// m_position (projected)
    output.m_position = mul(input.m_position, CB::g_model.m_modelViewProj);

	// Shadow mapping
    output.m_projectedCoord = mul(input.m_position, CB::g_shadow.m_shadowModelViewProj);

    return output;
}

float4 PosHRTransform(
    const float4 position
)
{
    return float4(
		mad(position.x, CB::g_pipeColor.m_packedPosition.x, CB::g_pipeColor.m_packedPosition.z),
		position.y + CB::g_pipeColor.m_packedPosition.y,
		position.z,
		1.0f);
}

float3 NormalHRTransform(
    const float3 pNormal
)
{
    return normalize(float3(pNormal.x * CB::g_pipeColor.m_packedPosition.w, pNormal.y, pNormal.z));
}