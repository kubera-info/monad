// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/cForecolor.hlsli"
#include "../Include/metal_pipe.hlsli"

template<typename INPUT_TYPE, typename OUTPUT_TYPE>
void CommonButtonCalcs(
    in INPUT_TYPE input, 
    out OUTPUT_TYPE output
)
{
    const float3 p = mul(input.m_position, (float4x3) CB::g_model.m_model); // position (view space)
    const float3 lNew = normalize(CB::g_lightPos.m_lightPosition - p);
    const float3 n = normalize(mul(input.m_normal, (float3x3) CB::g_model.m_model)); // normal (view space)
    const float3 v = -normalize(p); // view direction (view space)
    const float3 h = normalize(lNew + v); // half vector (view space)

// m_position (projected)
    output.m_position = mul(input.m_position, CB::g_model.m_modelViewProj);
// ArrayColors
    const float4 light = lit(dot(n, lNew), dot(n, h), 65.0);

    output.m_color = CB::g_forecolor.m_forecolor * light.y * .925 + light.z * .55;
// Shadow mapping
    output.m_projectedCoord = mul(input.m_position, CB::g_shadow.m_shadowModelViewProj);
}