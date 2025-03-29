#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class Material
{
public:

	Material(DirectX::XMFLOAT3 colorTint, float roughness,
		DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2(1, 1),
		DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2(0, 0));

	float GetRoughness();
	DirectX::XMFLOAT3 GetColorTint();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();

	D3D12_GPU_DESCRIPTOR_HANDLE GetFinalGPUHandleForSRVs();
	void AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot);
	void FinalizeMaterial();
	
private:

	DirectX::XMFLOAT3 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	float roughness = 1.f;

	bool finalized = false;
	int maxTextures = 4;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVsBySlot[4] {};
	D3D12_GPU_DESCRIPTOR_HANDLE finalGPUHandleForSRVs {};
};

