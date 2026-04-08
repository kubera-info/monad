// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/input.hlsli"
#include "../Include/output.hlsli"

VS_OUTPUT_FONT_SHADOW main(
    VS_PANE input
)
{
    VS_OUTPUT_FONT_SHADOW output;

	// Move model to the pixel shader
    output = (VS_OUTPUT_FONT_SHADOW) input;
    
    return output;
}