// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

namespace CB
{
    struct Forecolor
    {
        float3 m_forecolor;
        float m_pad;
    };
    ConstantBuffer<Forecolor> g_forecolor : register(b5);
}