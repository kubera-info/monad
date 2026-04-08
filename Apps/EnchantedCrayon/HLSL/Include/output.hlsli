// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

struct VS_OUTPUT_COLOR
{
    float4 m_position : SV_Position;
    float3 m_color : COLOR0;
    float4 m_projectedCoord : TEXCOORD0;
};

struct VS_OUTPUT_TEX
{
    float4 m_position : SV_Position;
    float2 m_texture : TEXCOORD0;
    float4 m_projectedCoord : TEXCOORD1;
};

struct VS_OUTPUT_METAL_PIPE
{
    float4 m_position : SV_Position;
    float3 m_lNew : TEXCOORD0;
    float3 m_h : TEXCOORD1;
    float3 m_n : TEXCOORD2;
    float m_f : TEXCOORD3;
    float4 m_projectedCoord : TEXCOORD4;
};

struct VS_OUTPUT_FONT
{
    float4 m_position : SV_Position;
    float3 m_color : COLOR0;
    float2 m_texture : TEXCOORD0;
    float4 m_projectedCoord : TEXCOORD1;
};

struct VS_OUTPUT_EMOJI
{
    float4 m_position : SV_Position;
    float2 m_texture : TEXCOORD0;
    float4 m_projectedCoord : TEXCOORD1;
};

struct VS_OUTPUT_FONT_SHADOW
{
    float4 m_position : SV_Position;
    float2 m_texture : TEXCOORD0;
};

struct VS_OUTPUT_ICONIC_BUTTON
{
    float4 m_position : SV_Position;
    float3 m_color : COLOR0;
    float2 m_texture : TEXCOORD0;
    float4 m_projectedCoord : TEXCOORD1;
};