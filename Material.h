#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class Material
{
public:

	Material(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
		DirectX::XMFLOAT3 colorTint,
		DirectX::XMFLOAT2 uvScale,
		DirectX::XMFLOAT2 uvOffset);

	DirectX::XMFLOAT3 GetColorTint();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();

	D3D12_GPU_DESCRIPTOR_HANDLE GetFinalGPUHandleForSRVs();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState();
	void AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot);
	void FinalizeMaterial();
	
private:

	DirectX::XMFLOAT3 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;

	bool finalized = false;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState; // replaces VS and PS in D3D11
	int maxTextures = 4;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVsBySlot[4];
	D3D12_GPU_DESCRIPTOR_HANDLE finalGPUHandleForSRVs;
};

