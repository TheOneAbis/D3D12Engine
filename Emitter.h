#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <vector>

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

	std::vector<Particle> particles;
	Microsoft::WRL::ComPtr<ID3D12Resource> particleBuffer;

	// Properties for particles
	unsigned int firstAlive, firstDead;
	unsigned int numAlive;

	//Emitter properties
	float emissionTmr;
};
