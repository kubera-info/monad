// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/metal_pipe.hlsli"

VS_OUTPUT_METAL_PIPE main(
    VS_INPUT input
)
{
    VS_INPUT output;

    output.m_position = PosHRTransform(input.m_position);
    output.m_normal = NormalHRTransform(input.m_normal);

    return metal(output);
}