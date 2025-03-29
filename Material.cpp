#include "Material.h"

#include "Graphics.h"

Material::Material(DirectX::XMFLOAT3 colorTint,
	float roughness,
	DirectX::XMFLOAT2 uvScale,
	DirectX::XMFLOAT2 uvOffset)
{
	this->roughness = roughness;
	this->colorTint = colorTint;
	this->uvScale = uvScale;
	this->uvOffset = uvOffset;

	finalGPUHandleForSRVs = {};
}

float Material::GetRoughness()
{
	return roughness;
}

DirectX::XMFLOAT3 Material::GetColorTint()
{
	return colorTint;
}
DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}
DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetFinalGPUHandleForSRVs()
{
	return finalGPUHandleForSRVs;
}

void Material::AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot)
{
	if (slot < 0 || slot >= maxTextures) return;
	textureSRVsBySlot[slot] = srv;
}

void Material::FinalizeMaterial()
{
	if (!finalized) // only finalize once
	{
		finalized = true;

		// save the first GPU descriptor handle here
		finalGPUHandleForSRVs = Graphics::CopySRVsToDescriptorHeapAndGetGPUDescriptorHandle(textureSRVsBySlot[0], 1);
		for (int i = 1; i < maxTextures; i++)
		{
			Graphics::CopySRVsToDescriptorHeapAndGetGPUDescriptorHandle(textureSRVsBySlot[i], 1);
		}
	}
}