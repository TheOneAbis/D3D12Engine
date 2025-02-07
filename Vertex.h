#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The local position of the vertex
	DirectX::XMFLOAT3 Normal;       // This vertex's normal
	DirectX::XMFLOAT2 UV;           // The UV coord of this vertex
	DirectX::XMFLOAT3 Tangent;      // Tangent for normal mapping
};