// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

namespace CB
{
    struct LightPos
    {
        float3 m_lightPosition;
        float m_pad;
    };
    ConstantBuffer<LightPos> g_lightPos : register(b1);
}