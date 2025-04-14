#include "VToP.hlsli"
#include "LightingFunctions.hlsli"

#define MAX_LIGHTS 10

cbuffer ExternalData : register(b0)
{
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    uint lightCount;
    float4 ambient;
    Light lights[MAX_LIGHTS];
}

Texture2D albedo    : register(t0);
Texture2D normal    : register(t1);
Texture2D metallic  : register(t2);
Texture2D roughness : register(t3);

SamplerState basicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    // scroll uv
    input.uv += uvOffset;

    // Calculate vector from surface to camera
    float3 viewVector = normalize(cameraPosition - input.worldPosition);

    // Renormalize from the map if using normal map
    float3 normalFromMap = normalize(normal.Sample(basicSampler, input.uv * uvScale).rgb * 2 - 1); // scale 0 to 1 values to -1 to 1
        
    // rotate normal map to convert from tangent to world space (since our input values are already in world space from VS)
    // Ensure we orthonormalize the tangent again
    float3 N = normalize(input.normal); // Normal
    float3 T = normalize(input.tangent); // Tangent
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt orthonomalizing of the Tangent
    float3 B = cross(T, N); // Bi-tangent
    float3x3 TBN = float3x3(T, B, N); // TBN rotation matrix

    // multiply normal map vector by the TBN matrix
    input.normal = mul(normalFromMap, TBN);

    // Sample the surface texture for the initial pixel color (scale texture if a scale was specified)
    // un-correct the albedo color w/ gamma value
    float3 albedoColor = pow(albedo.Sample(basicSampler, input.uv * uvScale).rgb, 2.2f);
    float metal = metallic.Sample(basicSampler, input.uv * uvScale).r;
    float rough = roughness.Sample(basicSampler, input.uv * uvScale).r;
    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(NONMETAL_F0, albedoColor.rgb, metal);
    float3 totalLightColor = ambient.rgb * albedoColor * (1 - metal);
    
    // Loop through the lights
    for (uint i = 0; i < lightCount; i++)
    {
        float3 lightDir = float3(0, 0, 0);
        bool attenuate = false;
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                lightDir = normalize(lights[i].Direction);
                break;
            case LIGHT_TYPE_POINT:
                lightDir = normalize(input.worldPosition - lights[i].Position);
                attenuate = true;
                break;
            case LIGHT_TYPE_SPOT:
                lightDir = normalize(input.worldPosition - lights[i].Position);
                attenuate = true;
                break;
        }
        
        float3 lightCol = ColorFromLightPBR(input.normal, -lightDir, lights[i].Color,
            albedoColor, viewVector, lights[i].Intensity, rough, metal, specularColor);

        // If this is a point or spot light, attenuate the color
        if (attenuate)
            lightCol *= Attenuate(lights[i], input.worldPosition);
        
        totalLightColor += lightCol;
    }
    
    // correct color w/ gamma and return final color
    return float4(pow(totalLightColor, 1.0f / 2.2f), 1);
}