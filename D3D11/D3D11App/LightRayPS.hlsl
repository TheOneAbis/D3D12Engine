#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"


cbuffer ExternalData : register(b0)
{
	// Scene related
    Light lights[MAX_LIGHTS];
	float2 lightUVs[MAX_LIGHTS];
	int lightCount;
    
    int numSamples;
    float exposure;
    float density;
    float weight;
    float decay;
}

// Texture-related resources
Texture2D SceneTexture : register(t0);
Texture2D WorldPositionTexture : register(t1);
Texture2D LightVisibilityTexture : register(t2);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float3 finalCol = SceneTexture.Sample(BasicSampler, input.uv);
    
    for (int i = 0; i < lightCount; i++)
    {
        float2 currentUV = input.uv;
        
        // Calculate vector from pixel to light source in screen space.
        float2 deltaTexCoord = (currentUV - lightUVs[i]);
        // Divide by number of samples and scale by control factor.
        deltaTexCoord *= 1.f / float(numSamples) * density;
        // Store initial sample.
        float3 color = LightVisibilityTexture.Sample(BasicSampler, currentUV);
        // Set up illumination decay factor.
        float illuminationDecay = 1.f;
        
        // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
        for (int i = 0; i < numSamples; i++)
        {
            // Step sample location along ray.
            currentUV -= deltaTexCoord;
            // Retrieve sample at new location.
            float3 sample = LightVisibilityTexture.Sample(BasicSampler, currentUV);
            // Apply sample attenuation scale/decay factors.
            sample *= illuminationDecay * weight;
            
            // My gut tells me the way to handle volumetric spot lights is in here somewhere.
            // Need to get the light's direction into screen space. Even then idk how accurate this will look
            //sample *= pow(saturate(dot(normalize(input.uv - lightUVs[i]), lights[i].Direction)), lights[i].SpotFalloff); // penumbra
            
            // Accumulate combined color.
            color += sample;
            // Update exponential decay factor.
            illuminationDecay *= decay;
        }
        
        // Add to final color with a further scale control factor.
        finalCol += color * exposure;
    }
  
    
    return float4(finalCol, 1);
}