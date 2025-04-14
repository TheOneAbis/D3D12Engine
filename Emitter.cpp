#include "Emitter.h"

using namespace std;

Emitter::Emitter(float maxLifetime, float emissionRate) : 
	maxLifetime(maxLifetime), emissionRate(emissionRate)
{
	// Make dynamic buffer to hold particles in GPU
	// Will be overwritten every frame with new lifetime data

}

void Emitter::Update(float deltaTime)
{

}

void Emitter::Draw()
{

}