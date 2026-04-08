// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/lookup.hlsli"
#include "../Include/output.hlsli"

float4 main(VS_OUTPUT_FONT_SHADOW input) : SV_TARGET
{
    return GetColorFromTexel2(input.m_texture);
}