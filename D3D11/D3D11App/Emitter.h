#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>

#include "SimpleShader.h"
#include "Transform.h"

class Camera;

struct Particle
{
	float emitTime;
	DirectX::XMFLOAT3 startPos;
};

class Emitter
{
public:

	Emitter(
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps, 
		float rate, float maxLifetime, bool additive, 
		Transform transform);
	~Emitter();

	void Update(float deltaTime);
	void Draw(std::shared_ptr<Camera> cam);

	void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void SetSampler(Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

	// emission properties
	float maxLifetime;
	float emissionRate;
	Transform transform;

private:

	// particle properties
	unsigned int maxParticles;
	Particle* particles;
	unsigned int numIndices;
	unsigned int firstAlive, firstDead, numAlive;

	float emissionTime;
	float emissionTmr;
	float currentTime;
	bool additive;

	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> particleDSS;
	Microsoft::WRL::ComPtr<ID3D11BlendState> particleBlendState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
};