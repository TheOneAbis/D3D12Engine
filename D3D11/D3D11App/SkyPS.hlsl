
#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"


cbuffer ExternalData : register(b0)
{
    Light lights[MAX_LIGHTS];
    int lightCount;
    float3 camPos;
    float falloff;
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
    
    float3 normalDir = normalize(input.sampleDir);
    float3 resultColor = float3(0, 0, 0);
    for (int i = 0; i < lightCount; i++)
    {
        float3 lightNormalDir = normalize(-lights[i].Direction);
        float3 lightNormalPos = normalize(lights[i].Position - camPos);
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                resultColor += pow(saturate(dot(lightNormalDir, normalDir)), falloff) * lights[i].Color;
                break;
            case LIGHT_TYPE_POINT:
                resultColor += pow(saturate(dot(lightNormalPos, normalDir)), falloff) * lights[i].Color;
                break;
            case LIGHT_TYPE_SPOT:
                resultColor += pow(saturate(dot(lightNormalPos, normalDir)), falloff) * lights[i].Color;
                break;
        }
    }
    
    output.lightVisColor = float4(resultColor, 1);
    return output;
}