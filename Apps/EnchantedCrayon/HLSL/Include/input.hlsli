// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

struct VS_INPUT
{
    float4 m_position : POSITION; // vertex position 
    float3 m_normal : NORMAL; // this normal comes in per-vertex
};

struct VS_INPUT_TEX
{
    float4 m_position : POSITION; // vertex position 
    float3 m_normal : NORMAL; // this normal comes in per-vertex
    float2 m_texture : TEXCOORD0; // vertex texture coords 
};

struct VS_FONT
{
    float2 m_position : POSITION; // vertex position
    float2 m_texture : TEXCOORD0; // vertex texture coords
    nointerpolation float3 m_color : COLOR0;
};

struct VS_EMOJI
{
    float2 m_position : POSITION; // vertex position 
    float2 m_texture : TEXCOORD0; // vertex texture coords
};

struct VS_PANE
{
    float4 m_position : POSITION; // vertex position 
    float2 m_texture : TEXCOORD0;
};