// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __FONT_VERT_HLSLI__
#define __FONT_VERT_HLSLI__

static float4 CalculateVertexPosition(
    float2 position
)
{
    return float4(position.xy, 0.012125, 1.0);
}
#endif