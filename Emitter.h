#pragma once

#include <d3d12.h>
#include <wrl/client.h>

struct Particle
{
	float time;
	float startPos;
};

class Emitter
{
public:

	Emitter(float maxLifetime, float emissionRate);
	void Update(float deltaTime);
	void Draw();

	// Emitter properties
	float maxLifetime;
	float emissionRate;
	float emissionTime;
	

private:

	int maxParticles;
	Particle* particles;
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer;

	// Properties for particles
	unsigned int firstAlive, firstDead, numAlive;

	//Emitter properties
	float emissionTmr;
};
