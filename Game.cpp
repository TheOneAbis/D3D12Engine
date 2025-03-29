#include "Game.h"
#include "Graphics.h"
#include "RayTracing.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#include "BufferStructs.h"

#define PI 3.1415926f

// For the DirectX Math library
using namespace DirectX;

inline float RandFloat(float min, float max)
{
	return min + (float)rand() / ((float)RAND_MAX / (max - min));
}

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Initialize raytracing
	RayTracing::Initialize(
		Window::Width(),
		Window::Height(),
		FixPath(L"RayTracing.cso"));

	// create meshes
	meshMap["SM_Cube"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/cube.obj").c_str());
	meshMap["SM_Helix"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/helix.obj").c_str());
	meshMap["SM_Sphere"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/sphere.obj").c_str());
	meshMap["SM_Torus"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/torus.obj").c_str());
	meshMap["SM_Plane"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/quad.obj").c_str());

	// create entities
	entities.push_back(Entity(meshMap["SM_Cube"], std::make_shared<Material>(XMFLOAT3(1, 0.5f, 1), 1.f)));
	entities.push_back(Entity(meshMap["SM_Helix"], std::make_shared<Material>(XMFLOAT3(0.25f, 1, 0.78f), 1.f)));
	entities.push_back(Entity(meshMap["SM_Torus"], std::make_shared<Material>(XMFLOAT3(0.9f, 0.4f, 0.7f), 1.f)));
	entities.push_back(Entity(meshMap["SM_Plane"], std::make_shared<Material>(XMFLOAT3(0.5f, 0.5f, 0.6f), 0.6f)));

	// set entity transforms
	entities[0].SetWorldTM(Transform(XMFLOAT3(-6, 0, 4)));
	entities[1].SetWorldTM(Transform(XMFLOAT3(-2, 0, 4)));
	entities[2].SetWorldTM(Transform(XMFLOAT3(2, 0, 4)));
	entities[3].SetWorldTM(Transform(XMFLOAT3(0, -2, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(20, 20, 20)));
	
	for (int i = 0; i < 12; i++)
	{
		Entity e(meshMap["SM_Sphere"], std::make_shared<Material>(XMFLOAT3(RandFloat(0, 1), RandFloat(0, 1), RandFloat(0, 1)), RandFloat(0, 1)));
		e.SetWorldTM(XMFLOAT3(RandFloat(-4, 4), RandFloat(-4, 4), RandFloat(2, 5)), { 0, RandFloat(0, PI * 2.f), 0 }, {0.5f, 0.5f, 0.5f});
		entities.push_back(e);
	}

	// Create a BLAS for a single mesh, then the TLAS for our “scene”
	RayTracing::CreateTopLevelAccelerationStructureForScene(entities);

	// Finalize any initialization and wait for the GPU
	// before proceeding to the game loop
	Graphics::CloseAndExecuteCommandList();
	Graphics::WaitForGPU();
	Graphics::ResetAllocatorAndCommandList(0);

	// Create camera
	cam = Camera();
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// Wait for the GPU before we shut down
	Graphics::WaitForGPU();
}

// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Resize raytracing output texture
	RayTracing::ResizeOutputUAV(Window::Width(), Window::Height());

	// Rezize camera projection matrix
	cam.UpdateProjectionMatrix((float)Window::Width(), (float)Window::Height());
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	Transform t = entities[0].GetWorldTM();
	t.Rotate(0, deltaTime, 0);
	entities[0].SetWorldTM(t);

	for (int i = 4; i < entities.size(); i++)
	{
		Transform t = entities[i].GetWorldTM();
		XMFLOAT3 pos;
		XMVECTOR vpos = XMLoadFloat3(&t.GetPosition());
		vpos += XMLoadFloat3(&t.GetForward()) * sinf(totalTime) * deltaTime;
		XMStoreFloat3(&pos, vpos);
		t.SetPosition(pos);
		entities[i].SetWorldTM(t);
	}

	// update camera state
	cam.Update(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Grab the current back buffer for this frame
	Microsoft::WRL::ComPtr<ID3D12Resource> currentBackBuffer =
		Graphics::BackBuffers[Graphics::SwapChainIndex()];

	// Update TLAS
	RayTracing::CreateTopLevelAccelerationStructureForScene(entities);

	// Perform ray trace (which also copies the results to the back buffer)
	RayTracing::Raytrace(cam, currentBackBuffer);

	// Present
	{
		// Must occur BEFORE present
		Graphics::CloseAndExecuteCommandList();

		// Present the current back buffer and move to the next one
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);
		Graphics::AdvanceSwapChainIndex();

		// Work ahead on the next frame; program will halt only if CPU is too far ahead of GPU
		Graphics::ResetAllocatorAndCommandList(Graphics::SwapChainIndex());
	}
}