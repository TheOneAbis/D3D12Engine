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
	DirectX::XMFLOAT3 startPos;
	float startRotation;
	DirectX::XMFLOAT3 velocity;
	float angularVelocity;
	float startScale;
	float endScale;
	float emitTime;
};

class Emitter
{
public:

	Emitter(
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps, 
		float rate, 
		float maxLifetime,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	~Emitter();

	void Update(float deltaTime);
	void Draw(std::shared_ptr<Camera> cam);

	// emission properties
	float maxLifetime;
	float emissionRate;
	DirectX::XMFLOAT3 minPos = {};
	DirectX::XMFLOAT3 maxPos = {};
	float minRotation = 0;
	float maxRotation = 0;
	float minScale = 1, maxScale = 1;
	float minEndScale = 1, maxEndScale = 1;
	DirectX::XMFLOAT3 minVelocity = {};
	DirectX::XMFLOAT3 maxVelocity = {};
	float minAngularVel = 0;
	float maxAngularVel = 0;
	DirectX::XMFLOAT3 startColor = { 1, 1, 1 };
	DirectX::XMFLOAT3 endColor = { 1, 1, 1 };
	DirectX::XMFLOAT3 acceleration = { 0, 0, 0 };
	unsigned int burstCount = 1;

private:

	// particle properties
	unsigned int maxParticles;
	Particle* particles;
	unsigned int firstAlive, firstDead, numAlive;

	float emissionTime;
	float emissionTmr;
	float currentTime;

	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
};