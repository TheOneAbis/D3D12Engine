#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"


cbuffer ExternalData : register(b0)
{
	// Scene related
	Light lights[MAX_LIGHTS];
	int lightCount;
}

// Texture-related resources
Texture2D SceneTexture : register(t0);
Texture2D LightVisibilityTexture : register(t1);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return SceneTexture.Sample(BasicSampler, input.uv);
 // // Calculate vector from pixel to light source in screen space.
	//half2 deltaTexCoord = (input.uv - ScreenLightPos.xy);
 // // Divide by number of samples and scale by control factor.
	//deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
 // // Store initial sample.
	//half3 color = tex2D(frameSampler, texCoord);
 // // Set up illumination decay factor.
	//half illuminationDecay = 1.0f;
 // // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
	//for (int i = 0; i < NUM_SAMPLES; i++)
	//{
 //   // Step sample location along ray.
	//	texCoord -= deltaTexCoord;
 //   // Retrieve sample at new location.
	//	half3 sample = tex2D(frameSampler, texCoord);
 //   // Apply sample attenuation scale/decay factors.
	//	sample *= illuminationDecay * Weight;
 //   // Accumulate combined color.
	//	color += sample;
 //   // Update exponential decay factor.
	//	illuminationDecay *= Decay;
	//}
 // // Output final color with a further scale control factor.
	//return float4(color * Exposure, 1);
}