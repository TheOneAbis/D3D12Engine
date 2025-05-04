
#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"


cbuffer ExternalData : register(b0)
{
	// Scene related
    Light lights[MAX_LIGHTS];
    int lightCount;
}


// Texture-related resources
TextureCube SkyTexture		: register(t0);
SamplerState BasicSampler	: register(s0);

struct PSOutput
{
    float4 color : SV_TARGET0;
    float4 lightVisColor : SV_TARGET1;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// --------------------------------------------------------
PSOutput main(VertexToPixel_Sky input)
{
    PSOutput output;
    
	// When we sample a TextureCube (like "skyTexture"), we need
	// to provide a direction in 3D space (a float3) instead of a uv coord
	output.color = SkyTexture.Sample(BasicSampler, input.sampleDir);
    output.lightVisColor = float4(pow(dot(lights[0].Direction, input.sampleDir), 2.f).xxx, 1);
    
    return output;
}