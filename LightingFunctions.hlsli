#ifndef __LIGHTING_FUNCTIONS__
#define __LIGHTING_FUNCTIONS__

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f

struct Light
{
    int Type; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction; // Directional and Spot lights need a direction
    float Range; // Point and Spot lights have a max range for attenuation
    float3 Position; // Point and Spot lights have a position in space
    float Intensity; // All lights need an intensity
    float3 Color; // All lights need a color
    float SpotFalloff; // Spot lights need a value to define their “cone” size
    float3 Padding; // Purposefully padding to hit the 16-byte boundary
};

float DiffuseBRDF(float3 normal, float3 dirToLight)
{
	// saturate(x) clamps a value between 0 and 1 (so it's a glorified clamp(x, 0, 1) basically)
    return saturate(dot(normal, dirToLight));
}

float SpecularBRDF(float3 normal, float3 lightDir, float3 viewVector, float roughness, float roughnessScale)
{
	// Get reflection of light bounsing off the surface 
    float3 refl = reflect(lightDir, normal);
    
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

	// Compare reflection against view vector, raising it to
	// a very high power to ensure the falloff to zero is quick
    return pow(saturate(dot(refl, viewVector)), specExponent) * roughnessScale;
}

float3 ColorFromLight(float3 normal, float3 lightDir, float3 lightColor, float3 colorTint, float3 viewVec, float roughness, float roughnessScale)
{
    // Calculate diffuse an specular values
    float diffuse = DiffuseBRDF(normal, -lightDir);
    float spec = SpecularBRDF(normal, lightDir, viewVec, roughness, roughnessScale);
    
    // Cut the specular if the diffuse contribution is zero
    // - any() returns 1 if any component of the param is non-zero
    // - In this case, diffuse is a single float value
    // - Meaning any() returns 1 if diffuse itself is non-zero
    // - In other words:
    // - If the diffuse amount is 0, any(diffuse) returns 0
    // - If the diffuse amount is != 0, any(diffuse) returns 1
    // - So when diffuse is 0, specular becomes 0
    spec *= any(diffuse);

    return lightColor * colorTint * diffuse + spec;
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

// -------------------------------------------------------- \\
// -- PHYSICALLY-BASED RENDERING FUNCTIONS AND CONSTANTS -- \\
// -------------------------------------------------------- \\

// The fresnel value for non-metals (dielectrics)
// Page 9: "F0 of nonmetals is now a constant 0.04"
static const float NONMETAL_F0 = 0.04f;
static const float MIN_ROUGHNESS = 0.0000001f;
static const float PI = 3.14159265359f;

// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// F - Fresnel result from microfacet BRDF
// metalness - surface metalness amount
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

// Normal Distribution Function: GGX (Trowbridge-Reitz)
// a - Roughness
// h - Half vector: (V + L)/2
// n - Normal
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
	// Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	// Can go to zero if roughness is 0 and NdotH is 1
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}

// Fresnel term - Schlick approx.
// v - View vector
// h - Half vector
// f0 - Value when l = n
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
// n - Normal
// v - View vector
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
	// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}

// Cook-Torrance Microfacet BRDF (Specular)
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
    float3 h = normalize(v + l);

	// Run numerator functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
	
    F_out = F;

	// Final specular formula
	// Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
	// canceled out by our G() term.  As such, they have been removed
	// from BOTH places to prevent floating point rounding errors.
    float3 specularResult = (D * F * G) / 4;

	// One last non-obvious requirement: According to the rendering equation,
	// specular must have the same NdotL applied as diffuse!  We'll apply
	// that here so that minimal changes are required elsewhere.
    return specularResult * max(dot(n, l), 0);
}



float3 ColorFromLightPBR(float3 normal, float3 toLight, float3 lightColor, float3 surfaceColor,
    float3 viewVec, float lightIntensity, float roughness, float metalness, float3 specColor)
{
    // Diffuse is unchanged from non-PBR
    float diffuse = DiffuseBRDF(normal, toLight);
    
    float3 fresnel;
    // Get specular color and fresnel result
    float3 spec = MicrofacetBRDF(normal, toLight, viewVec, roughness, specColor, fresnel);
    
    // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diffuse, fresnel, metalness);
    
    // Combine the final diffuse and specular values for this light
    return (balancedDiff * surfaceColor + spec) * lightIntensity * lightColor;
}

#endif
