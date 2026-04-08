// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/metal_pipe.hlsli"
#include "../Include/shadow_common.hlsli"

VS_OUTPUT_MAP main(
    VS_INPUT input
)
{
    VS_OUTPUT_MAP output;

    output.m_position = mul(input.m_position, CB::g_shadow.m_shadowModelViewProj);

    return output;
}