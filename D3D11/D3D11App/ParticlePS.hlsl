
struct VToP
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float age : AGE;
    float lifespan : LIFESPAN;
};

cbuffer ExternalData : register(b0)
{
    float3 startColor;
    float padding;
    float3 endColor;
};

// Texture related resources
Texture2D colorTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VToP input) : SV_TARGET
{
    float agePercent = input.age / input.lifespan;
    return colorTexture.Sample(BasicSampler, input.uv) * float4(lerp(startColor, endColor, agePercent), 1.f - agePercent);
}