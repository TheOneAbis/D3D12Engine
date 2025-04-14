#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_map>
#include <string>

#include "Camera.h"
#include "Entity.h"
#include "Emitter.h"
#include "Light.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateRootSigAndPipelineState();
	void CreateParticleRootSigPipelineState();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Pipeline
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, particleRootSig;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState, particlePipelineState;

	// Other graphics data
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	Camera cam;
	std::vector<Entity> entities;
	std::vector<Emitter> emitters;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> meshMap;
	std::unordered_map<std::string, std::shared_ptr<Material>> materialMap;
	std::vector<Light> lights;
};

