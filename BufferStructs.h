#pragma once

#include <DirectXMath.h>
#include "Light.h"

#define MAX_LIGHTS 10

struct VSExternalData
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
};

struct PSExternalData
{
    DirectX::XMFLOAT2 uvScale;
    DirectX::XMFLOAT2 uvOffset;
    DirectX::XMFLOAT3 cameraPosition;
    unsigned int lightCount;
    DirectX::XMFLOAT4 ambient;
    Light lights[MAX_LIGHTS];
};

struct ParticleVSExternalData
{
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    float time;
};