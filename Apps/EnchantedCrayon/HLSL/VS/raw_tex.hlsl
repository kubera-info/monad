// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/input.hlsli"
#include "../Include/output.hlsli"
#include "../Include/cWorldViewProj.hlsli"
#include "../Include/cModel.hlsli"

VS_OUTPUT_TEX main(
    VS_PANE input
)
{
    VS_OUTPUT_TEX output;

	// Move model to the pixel shader
    output.m_position = mul(input.m_position, CB::g_model.m_modelViewProj);
    output.m_texture = input.m_texture;

	// transform model-space vertex position to light-space:
    output.m_projectedCoord = mul(input.m_position, CB::g_shadow.m_shadowModelViewProj);

    return output;
}