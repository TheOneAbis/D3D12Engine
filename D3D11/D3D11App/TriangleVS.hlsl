#include "ShaderStructs.hlsli"


VertexToPixel main(uint id : SV_VERTEXID)
{
    VertexToPixel output;
    // Calculate the UV (0,0 to 2,2) via the ID
    // x = 0, 2, 0, 2, etc.
    // y = 0, 0, 2, 2, etc.
    output.uv = float2((id << 1) & 2, id & 2);
    // Convert uv to the (-1,1 to 3,-3) range for position
    output.screenPosition = float4(output.uv, 0, 1);
    output.screenPosition.x = output.screenPosition.x * 2 - 1;
    output.screenPosition.y = output.screenPosition.y * -2 + 1;
    return output;
}
