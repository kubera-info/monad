// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#ifndef __COLOR_SPACE_H__
#define __COLOR_SPACE_H__

static inline float LinearToR(
    float color
)
{
    return color < 0.0031308
        ? 12.92 * color
        : 1.055 * pow(abs(color), 1.0 / 2.4) - 0.055;
}

static float3 LinearToSRGB(
    float3 color
)
{
    return float3(
        LinearToR(color.r),
        LinearToR(color.g),
        LinearToR(color.b));
}

static float4 LinearToSRGBA(
    float4 color
)
{
    return float4(LinearToSRGB(color.rgb), color.a);
}

static inline float SRToLinear(
    float color
)
{
    return color < 0.04045 
        ? color / 12.92
        : pow(abs(color + 0.055) / 1.055,
        2.4);
}

static float3 SRGBToLinear(
    float3 color
)
{
    return float3(
        SRToLinear(color.r),
        SRToLinear(color.g),
        SRToLinear(color.b));
}

static float4 SRGBAToLinear(
    float4 color
)
{
    return float4(SRGBToLinear(color.rgb), color.a);
}

static float3 Rec709ToRec2020(
    float3 color
)
{
    static const float3x3 conversion =
    {
        0.627402, 0.329292, 0.043306,
        0.069095, 0.919544, 0.011360,
        0.016394, 0.088028, 0.895578
    };
    return mul(conversion, color);
}

static float3 Rec2020ToRec709(
    float3 color
)
{
    static const float3x3 conversion =
    {
        1.660496, -0.587656, -0.072840,
        -0.124547, 1.132895, -0.008348,
        -0.018154, -0.100597, 1.118751
    };
    return mul(conversion, color);
}

static float3 LinearToST2084(
    float3 color
)
{
    float m1 = 2610.0 / 4096.0 / 4;
    float m2 = 2523.0 / 4096.0 * 128;
    float c1 = 3424.0 / 4096.0;
    float c2 = 2413.0 / 4096.0 * 32;
    float c3 = 2392.0 / 4096.0 * 32;
    float3 cp = pow(abs(color), m1);
    return pow((c1 + c2 * cp) / (1 + c3 * cp), m2);
}
#endif