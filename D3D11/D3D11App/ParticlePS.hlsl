
struct VToP
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer ExternalData : register(b0)
{
    float3 colorTint;
};

// Texture related resources
Texture2D colorTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VToP input) : SV_TARGET
{
    return colorTexture.Sample(BasicSampler, input.uv) * float4(colorTint, 1);
}