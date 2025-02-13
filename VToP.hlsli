#ifndef __V2P__
#define __V2P__

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float3 normal : NORMAL; // XYZ normal
    float2 uv : TEXCOORD; // UVs
    float3 tangent : TANGENT; // XYZ tangent
    float3 worldPosition : WORLDPOSITION; // world XYZ position
};

#endif