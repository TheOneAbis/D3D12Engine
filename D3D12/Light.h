#pragma once

#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

// Note SimpleShader handles boundary issues from separate data, but still need to manually handle this inside a struct
struct Light
{
	int Type = LIGHT_TYPE_DIRECTIONAL;         // Which kind of light? 0, 1 or 2 (see above)
	DirectX::XMFLOAT3 Direction = { 1, 0, 0 }; // Directional and Spot lights need a direction
	float Range = 1.f;                         // Point and Spot lights have a max range for attenuation
	DirectX::XMFLOAT3 Position = { 0, 0, 0 };  // Point and Spot lights have a position in space
	float Intensity = 1.f;                     // All lights need an intensity
	DirectX::XMFLOAT3 Color = { 1, 1, 1 };     // All lights need a color
	float SpotFalloff = 2.f;                   // Spot lights need a value to define their “cone” size
	DirectX::XMFLOAT3 Padding = { 0, 0, 0 };   // Purposefully padding to hit the 16-byte boundery
};