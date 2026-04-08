// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __HDR_SETTINGS_H__
#define __HDR_SETTINGS_H__

#include "color_space.hlsli"

namespace CB
{
    struct HDR
    {
        bool m_isHDR; // Toggle for HDR mode
        float3 m_pad;
    };
    ConstantBuffer<HDR> g_hdr : register(b0);
}

static float3 ConvertToLinear(const float3 color)
{
    if (CB::g_hdr.m_isHDR)
        // HDR input is already in linear space
        return color;
    else
        // Convert sRGB to linear space
        return SRGBToLinear(color);
}

static float3 ConvertToOutput(const float3 color)
{
    if (CB::g_hdr.m_isHDR)
        return LinearToST2084(color);
    else
        // Apply gamma correction for sRGB
        return LinearToSRGB(color);
}

static float4 ConvertToOutput(const float4 color)
{
    return float4(ConvertToOutput(color.rgb), color.a);
}
#endif