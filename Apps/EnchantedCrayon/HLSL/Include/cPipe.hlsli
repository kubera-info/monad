// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

namespace CB
{
    struct PipeColor
    {
        float3 m_diffuseColor;
        float m_pad1;
        float3 m_color;
        float m_pad2;
        float3 m_fresnelColor;
        float m_pad3;
        float4 m_packedPosition;
    };
    ConstantBuffer<PipeColor> g_pipeColor : register(b4);
}