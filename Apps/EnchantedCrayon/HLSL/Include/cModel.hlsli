// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

namespace CB
{
    struct Model
    {
        float4x4 m_modelViewProj;
        float4x4 m_model;
    };
    ConstantBuffer<Model> g_model : register(b3);
}