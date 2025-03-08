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

// Overall scene data for raytracing
struct RaytracingSceneData
{
    DirectX::XMFLOAT4X4 inverseViewProjection;
    DirectX::XMFLOAT3 cameraPosition;
    float pad;
};

// Ensure this matches Raytracing shader define!
#define MAX_INSTANCES_PER_BLAS 100
struct RaytracingEntityData
{
    DirectX::XMFLOAT4 color[MAX_INSTANCES_PER_BLAS];
};