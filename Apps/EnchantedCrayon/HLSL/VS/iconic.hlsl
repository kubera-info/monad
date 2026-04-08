// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/button_generic.hlsli"

VS_OUTPUT_ICONIC_BUTTON main(
    VS_INPUT_TEX input
)
{
    VS_OUTPUT_ICONIC_BUTTON output;

    CommonButtonCalcs(input, output);
    output.m_texture = input.m_texture;

    return output;
}