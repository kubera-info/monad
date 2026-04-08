// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/shadow_common.hlsli"

float main(
    VS_OUTPUT_MAP input
) : SV_DEPTH
{
    return input.m_position.z;
}