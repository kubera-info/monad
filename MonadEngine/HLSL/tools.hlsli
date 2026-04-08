// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __TOOLS_HLSLI__
#define __TOOLS_HLSLI__

static float3 GetMonochromeFromRGB(
    const float3 color
)
{
    // https://stackoverflow.com/questions/49863351/how-to-convert-image-to-greyscale-opengl-c
    return dot(color, float3(0.299, 0.587, 0.114));
}

static float3 FromFloat(
    const float3 brightColor, 
    const float lerpFactor
)
{
    return lerp(float3(
        0.0f, 0.0f, 0.0f), 
        brightColor, 
        lerpFactor
    );
}
#endif