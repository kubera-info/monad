// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/button_generic.hlsli"

VS_OUTPUT_COLOR main(
    VS_INPUT input
)
{
    VS_OUTPUT_COLOR output;

    CommonButtonCalcs(input, output);

    return output;
}