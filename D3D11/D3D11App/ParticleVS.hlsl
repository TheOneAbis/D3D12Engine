cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
    float currentTime;
}

struct Particle
{
    float emitTime;
    float3 startPos;
};

StructuredBuffer<Particle> particles : register(t0);

struct VToP
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// The entry point for our particle-specific vertex shader
// Only input is a single unique index for each vertex
VToP main(uint id : SV_VertexID)
{
    VToP output;
    
    uint particleID = id / 4; // Every group of 4 verts are ONE particle! (int division)
    uint cornerID = id % 4; // 0,1,2,3 = which corner of the particle’s "quad"
    Particle p = particles.Load(particleID); // Each vertex gets associated particle!

    // Offsets for the 4 corners of a quad - we'll only use one for each
    // vertex, but which one depends on the cornerID
    float2 offsets[4];
    offsets[0] = float2(-1.0f, +1.0f); // TL
    offsets[1] = float2(+1.0f, +1.0f); // TR
    offsets[2] = float2(+1.0f, -1.0f); // BR
    offsets[3] = float2(-1.0f, -1.0f); // BL
    
    float age = currentTime - p.emitTime;
    float3 pos = lerp(p.startPos, p.startPos + float3(0, 3, 0), age);
    
    // Billboarding: Offset the position based on the camera's right and up vectors
    pos += float3(view._11, view._12, view._13) * offsets[cornerID].x; // RIGHT
    pos += float3(view._21, view._22, view._23) * offsets[cornerID].y; // UP
    
    // Finally, calculate output position here using View and Projection matrices
    output.position = mul(mul(projection, view), float4(pos, 1));
    
    float2 uvs[4];
    uvs[0] = float2(0, 0); // TL
    uvs[1] = float2(1, 0); // TR
    uvs[2] = float2(1, 1); // BR
    uvs[3] = float2(0, 1); // BL
    output.uv = uvs[cornerID];
    
    return output;
}