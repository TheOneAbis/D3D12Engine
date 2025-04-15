cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
    float3 acceleration;
    float currentTime;
    float lifespan;
}

struct Particle
{
    float3 startPos;
    float startRotation;
    float3 velocity;
    float angularVelocity;
    float startScale;
    float endScale;
    float emitTime;
};

StructuredBuffer<Particle> particles : register(t0);

struct VToP
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float age : AGE;
    float lifespan : LIFESPAN;
};

float Rand(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453123f);
}

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
    float rot = p.startRotation + p.angularVelocity * p.emitTime;
    float2x2 rotMat = float2x2(cos(rot), -sin(rot),
                               sin(rot), cos(rot));
    float2x2 scaleMat = lerp(float2x2(p.startScale, 0, 0, p.startScale),
                             float2x2(p.endScale, 0, 0, p.endScale), p.emitTime / lifespan);
    
    float2 offsets[4];
    offsets[0] = mul(mul(scaleMat, rotMat), float2(-1.0f, +1.0f)); // TL
    offsets[1] = mul(mul(scaleMat, rotMat), float2(+1.0f, +1.0f)); // TR
    offsets[2] = mul(mul(scaleMat, rotMat), float2(+1.0f, -1.0f)); // BR
    offsets[3] = mul(mul(scaleMat, rotMat), float2(-1.0f, -1.0f)); // BL
    
    float3 pos = p.startPos + p.velocity * p.emitTime + acceleration * p.emitTime * p.emitTime / 2.f;
    
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
    
    output.age = p.emitTime;
    output.lifespan = lifespan;
    
    return output;
}