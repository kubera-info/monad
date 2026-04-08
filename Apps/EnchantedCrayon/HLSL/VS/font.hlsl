// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "../Include/input.hlsli"
#include "../Include/output.hlsli"
#include "../Include/cWorldViewProj.hlsli"
#include "../Include/cModel.hlsli"
#include "../Include/cForecolor.hlsli"
#include "../Include/font_vert.hlsli"
#include "../../../../MonadEngine/HLSL/tools.hlsli"

VS_OUTPUT_FONT main(
    VS_FONT input
)
{
    VS_OUTPUT_FONT output;
    
    const float4 position = CalculateVertexPosition(input.m_position);
    output.m_position = mul(position, CB::g_model.m_modelViewProj);
    output.m_texture = input.m_texture;
    output.m_color = lerp(GetMonochromeFromRGB(input.m_color), input.m_color, CB::g_forecolor.m_forecolor.r);
	// transform model-space vertex position to light-space:
    output.m_projectedCoord = mul(position, CB::g_shadow.m_shadowModelViewProj);

    return output;
}