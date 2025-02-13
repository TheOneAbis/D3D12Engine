#include "VToP.hlsli"

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;       // XYZ normal
    float2 uv               : TEXCOORD;     // UVs
    float3 tangent          : TANGENT;      // XYZ tangent
};

cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix worldInvTranspose;
    matrix view;
    matrix projection;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
    VertexToPixel output;
	
	// Multiply the three matrices together first
    matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

	// world position of the vertex
    output.worldPosition = mul(world, float4(input.localPosition, 1.0f)).xyz;
	
	// bring normal and tangent into world space
    output.normal = mul((float3x3) worldInvTranspose, input.normal);
    output.tangent = mul((float3x3) world, input.tangent);

	// Pass UV to PS
    output.uv = input.uv;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}