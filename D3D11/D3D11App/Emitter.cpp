#include "Emitter.h"
#include "Graphics.h"
#include "Camera.h"

#include <iostream>

Emitter::Emitter(
	std::shared_ptr<SimpleVertexShader> vs,
	std::shared_ptr<SimplePixelShader> ps,
	float rate, float maxLifetime, Transform transform,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler) :
	vs(vs), 
	ps(ps), 
	transform(transform),
	emissionRate(rate),
	maxLifetime(maxLifetime),
	textureSRV(srv),
	sampler(sampler)
{
	emissionTime = 1.f / emissionRate;
	emissionTmr = emissionTime;

	firstAlive = 0;
	firstDead = 0;
	numAlive = 0;
	currentTime = 0;

	// initialize dead particles
	maxParticles = 100;
	particles = new Particle[maxParticles];

	// create index buffer
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (unsigned int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	
	// Create the index buffer
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned int) * (UINT)maxParticles * 6; // Number of indices
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	Graphics::Device->CreateBuffer(&ibd, &indexData, indexBuffer.GetAddressOf());
	delete[] indices;

	// Make a dynamic buffer to hold all particle data on GPU
	// Note: We'll be overwriting this every frame with new lifetime data
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(Particle);
	desc.ByteWidth = sizeof(Particle) * maxParticles;
	Graphics::Device->CreateBuffer(&desc, 0, particleBuffer.GetAddressOf());

	// Create an SRV that points to a structured buffer of particles
	// so we can grab this data in a vertex shader
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	Graphics::Device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, particleSRV.GetAddressOf());
}

Emitter::~Emitter()
{
	delete[] particles;
}

void Emitter::Update(float dt)
{
	currentTime += dt;

	// Kill old particles
	unsigned int died = 0;
	for (unsigned int i = 0; i < numAlive; i++)
	{
		Particle* p = &particles[(i + firstAlive) % maxParticles];
		p->emitTime += dt;

		if (p->emitTime >= maxLifetime)
		{
			died++;
		}
	}
	numAlive -= died;
	firstAlive = (firstAlive + died) % maxParticles;

	// Spawn new particles
	emissionTmr += dt;
	while (emissionTmr > emissionTime)
	{
		particles[firstDead].emitTime = 0;
		particles[firstDead].startPos = transform.GetPosition();

		numAlive++;
		firstDead = (firstDead + 1) % maxParticles;

		emissionTmr -= emissionTime;
	}

	std::cout << numAlive << std::endl;
}

void Emitter::Draw(std::shared_ptr<Camera> cam)
{
	// Map the buffer, locking it on the GPU so we can write to it
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	Graphics::Context->Map(particleBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	// How are living particles arranged in the buffer?
	if (firstAlive < firstDead)
	{
		// Copy FirstAlive -> FirstDead
		memcpy(mapped.pData, particles + firstAlive, sizeof(Particle) * numAlive);
	}
	else
	{
		// Copy 0 -> FirstDead
		memcpy(mapped.pData, particles, sizeof(Particle) * firstDead);
		// Copy FirstAlive -> End into pData starting at the end of previous memcpy()
		memcpy((void*)((Particle*)mapped.pData + firstDead), particles + firstAlive, sizeof(Particle) * (maxParticles - firstAlive));
	}

	// Unmap (unlock) now that we're done with it
	Graphics::Context->Unmap(particleBuffer.Get(), 0);

	// Draw
	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* junk = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, &junk, &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	vs->SetShader();
	ps->SetShader();
	
	// Send data to the vertex shader
	vs->SetMatrix4x4("view", cam->GetView());
	vs->SetMatrix4x4("projection", cam->GetProjection());
	vs->SetFloat("currentTime", currentTime);
	vs->CopyAllBufferData();

	vs->SetShaderResourceView("particles", particleSRV.Get());

	// Send data to the pixel shader
	ps->SetFloat3("colorTint", { 1, 1, 1 });
	ps->CopyAllBufferData();

	ps->SetShaderResourceView("colorTexture", textureSRV.Get());
	ps->SetSamplerState("BasicSampler", sampler.Get());

	Graphics::Context->DrawIndexed(numAlive * 6, 0, 0);
}