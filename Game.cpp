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

// For the DirectX Math library
using namespace DirectX;

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

	//// create entities
	//entities.push_back(Entity(meshMap["SM_Cube"], nullptr));
	//entities.push_back(Entity(meshMap["SM_Helix"], nullptr));
	//entities.push_back(Entity(meshMap["SM_Sphere"], nullptr));
	//entities.push_back(Entity(meshMap["SM_Torus"], nullptr));

	//// set entity transforms
	//entities[0].SetWorldTM(Transform(XMFLOAT3(-6, 0, 4)));
	//entities[1].SetWorldTM(Transform(XMFLOAT3(-2, 0, 4)));
	//entities[2].SetWorldTM(Transform(XMFLOAT3(2, 0, 4)));
	//entities[3].SetWorldTM(Transform(XMFLOAT3(6, 0, 4)));

	// Create a BLAS for a single mesh, then the TLAS for our “scene”
	RayTracing::CreateBottomLevelAccelerationStructureForMesh(meshMap["SM_Sphere"].get());
	RayTracing::CreateTopLevelAccelerationStructureForScene();

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