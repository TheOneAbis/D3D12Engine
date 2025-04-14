struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
    float time;
};

struct Particle
{
    float time;
    float startPos;
};

StructuredBuffer<Particle> particles : register(t0);