// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

namespace CB
{
    struct Shadow
    {
        float4x4 m_shadowModelViewProj;
    };
    ConstantBuffer<Shadow> g_shadow : register(b2);
}