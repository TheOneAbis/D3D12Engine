#include "Game.h"
#include "Graphics.h"
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
	CreateRootSigAndPipelineState();
	CreateParticleRootSigPipelineState();

	// create meshes
	meshMap["SM_Cube"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/cube.obj").c_str());
	meshMap["SM_Helix"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/helix.obj").c_str());
	meshMap["SM_Sphere"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/sphere.obj").c_str());
	meshMap["SM_Torus"] = std::make_shared<Mesh>(FixPath(L"../../Assets/Basic Meshes/torus.obj").c_str());
	
	// create materials
	// wood
	std::shared_ptr<Material> mat = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1), XMFLOAT2(1, 1), XMFLOAT2(0, 0));
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/wood_albedo.png").c_str()), 0);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/wood_normals.png").c_str()), 1);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/wood_metal.png").c_str()), 2);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/wood_roughness.png").c_str()), 3);
	mat->FinalizeMaterial();
	materialMap["M_Wood"] = mat;

	// paint
	mat = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1), XMFLOAT2(1, 1), XMFLOAT2(0, 0));
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/paint_albedo.png").c_str()), 0);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/paint_normals.png").c_str()), 1);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/paint_metal.png").c_str()), 2);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/paint_roughness.png").c_str()), 3);
	mat->FinalizeMaterial();
	materialMap["M_Paint"] = mat;

	// rock
	mat = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1), XMFLOAT2(1, 1), XMFLOAT2(0, 0));
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/rough_albedo.png").c_str()), 0);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/rough_normals.png").c_str()), 1);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/rough_metal.png").c_str()), 2);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/rough_roughness.png").c_str()), 3);
	mat->FinalizeMaterial();
	materialMap["M_Rock"] = mat;

	// scratched
	mat = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1), XMFLOAT2(1, 1), XMFLOAT2(0, 0));
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/scratched_albedo.png").c_str()), 0);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/scratched_normals.png").c_str()), 1);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/scratched_metal.png").c_str()), 2);
	mat->AddTexture(Graphics::LoadTexture(FixPath(L"../../Assets/PBR/scratched_roughness.png").c_str()), 3);
	mat->FinalizeMaterial();
	materialMap["M_Scratched"] = mat;

	// create entities
	entities.push_back(Entity(meshMap["SM_Cube"], materialMap["M_Wood"]));
	entities.push_back(Entity(meshMap["SM_Helix"], materialMap["M_Paint"]));
	entities.push_back(Entity(meshMap["SM_Sphere"], materialMap["M_Rock"]));
	entities.push_back(Entity(meshMap["SM_Torus"], materialMap["M_Scratched"]));

	// set entity transforms
	entities[0].SetWorldTM(Transform(XMFLOAT3(-6, 0, 4)));
	entities[1].SetWorldTM(Transform(XMFLOAT3(-2, 0, 4)));
	entities[2].SetWorldTM(Transform(XMFLOAT3(2, 0, 4)));
	entities[3].SetWorldTM(Transform(XMFLOAT3(6, 0, 4)));

	// Create camera
	cam = Camera();

	// Create lights
	Light dirLight;
	dirLight.Type = LIGHT_TYPE_DIRECTIONAL;
	dirLight.Direction = { -0.5f, -1, 0 };
	dirLight.Color = { 1, 1, 1 };
	dirLight.Intensity = 1.f;
	lights.push_back(dirLight);

	Light point;
	point.Type = LIGHT_TYPE_POINT;
	point.Position = { -1.f, -0.5f, 0.8f };
	point.Color = { 1, 0, 1 };
	point.Intensity = 5.f;
	point.Range = 4.f;
	lights.push_back(point);

	point.Position = { 3, 0.75f, 0.5f };
	point.Color = { 0, 1, 1 };
	lights.push_back(point);
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
// Loads the two basic shaders, then creates the root signature
// and pipeline state object for our very basic demo.
// --------------------------------------------------------
void Game::CreateRootSigAndPipelineState()
{
	// Blobs to hold raw shader byte code used in several steps below
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;

	// Load shaders
	{
		// Read our compiled vertex shader code into a blob
		// - Essentially just "open the file and plop its contents here"
		D3DReadFileToBlob(
			FixPath(L"VertexShader.cso").c_str(), vertexShaderByteCode.GetAddressOf());
		D3DReadFileToBlob(
			FixPath(L"PixelShader.cso").c_str(), pixelShaderByteCode.GetAddressOf());
	}

	// Input layout
	const unsigned int inputElementCount = 4;
	D3D12_INPUT_ELEMENT_DESC inputElements[inputElementCount] = {};
	{
		// Create an input layout that describes the vertex format
		// used by the vertex shader we're using
		// - This is used by the pipeline to know how to interpret the raw data
		// sitting inside a vertex buffer

		// position
		inputElements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // float3
		inputElements[0].SemanticName = "POSITION";            // must match semantic in shader
		inputElements[0].SemanticIndex = 0;                    // first POSITION semantic

		// normal
		inputElements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[1].SemanticName = "NORMAL";
		inputElements[1].SemanticIndex = 0;

		// uv
		inputElements[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElements[2].SemanticName = "TEXCOORD";
		inputElements[2].SemanticIndex = 0;

		// tangent
		inputElements[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[3].SemanticName = "TANGENT";
		inputElements[3].SemanticIndex = 0;
	}

	// Root Signature
	{
		// Describe the range of CBVs needed for the vertex shader
		D3D12_DESCRIPTOR_RANGE cbvRangeVS = {};
		cbvRangeVS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangeVS.NumDescriptors = 1;
		cbvRangeVS.BaseShaderRegister = 0;
		cbvRangeVS.RegisterSpace = 0;
		cbvRangeVS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Describe the range of CBVs needed for the pixel shader
		D3D12_DESCRIPTOR_RANGE cbvRangePS = {};
		cbvRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangePS.NumDescriptors = 1;
		cbvRangePS.BaseShaderRegister = 0;
		cbvRangePS.RegisterSpace = 0;
		cbvRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create a range of SRV's for textures
		D3D12_DESCRIPTOR_RANGE srvRange = {};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 4; // Set to max number of textures at once (match pixel shader!)
		srvRange.BaseShaderRegister = 0; // Starts at s0 (match pixel shader!)
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create the root parameters
		D3D12_ROOT_PARAMETER rootParams[3] = {};

		// CBV table param for vertex shader
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[0].DescriptorTable.pDescriptorRanges = &cbvRangeVS;

		// CBV table param for pixel shader
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[1].DescriptorTable.pDescriptorRanges = &cbvRangePS;

		// SRV table param
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[2].DescriptorTable.pDescriptorRanges = &srvRange;

		// Create a single static sampler (available to all pixel shaders at the same slot)
		D3D12_STATIC_SAMPLER_DESC anisoWrap = {};
		anisoWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.Filter = D3D12_FILTER_ANISOTROPIC;
		anisoWrap.MaxAnisotropy = 16;
		anisoWrap.MaxLOD = D3D12_FLOAT32_MAX;
		anisoWrap.ShaderRegister = 0; // register(s0)
		anisoWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_STATIC_SAMPLER_DESC samplers[] = { anisoWrap };

		// Describe the full root signature
		D3D12_ROOT_SIGNATURE_DESC rootSig = {};
		rootSig.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSig.NumParameters = ARRAYSIZE(rootParams);
		rootSig.pParameters = rootParams;
		rootSig.NumStaticSamplers = ARRAYSIZE(samplers);
		rootSig.pStaticSamplers = samplers;

		ID3DBlob* serializedRootSig = 0;
		ID3DBlob* errors = 0;

		D3D12SerializeRootSignature(
			&rootSig,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSig,
			&errors);

		// Check for errors during serialization
		if (errors != 0)
		{
			OutputDebugString((wchar_t*)errors->GetBufferPointer());
		}

		// Actually create the root sig
		Graphics::Device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(rootSignature.GetAddressOf()));
	}

	// Pipeline state
	{
		// Describe the pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		// -- Input assembler related ---
		psoDesc.InputLayout.NumElements = inputElementCount;
		psoDesc.InputLayout.pInputElementDescs = inputElements;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Overall primitive topology type (triangle, line, etc.) is set here
		// IASetPrimTop() is still used to set list/strip/adj options

		// Root sig
		psoDesc.pRootSignature = rootSignature.Get();

		// -- Shaders (VS/PS) ---
		psoDesc.VS.pShaderBytecode = vertexShaderByteCode->GetBufferPointer();
		psoDesc.VS.BytecodeLength = vertexShaderByteCode->GetBufferSize();
		psoDesc.PS.pShaderBytecode = pixelShaderByteCode->GetBufferPointer();
		psoDesc.PS.BytecodeLength = pixelShaderByteCode->GetBufferSize();

		// -- Render targets ---
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;

		// -- States ---
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.DepthClipEnable = true;
		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;

		// -- Misc ---
		psoDesc.SampleMask = 0xffffffff;

		// Create the pipe state object
		Graphics::Device->CreateGraphicsPipelineState(
			&psoDesc,
			IID_PPV_ARGS(pipelineState.GetAddressOf()));
	}

	// Set up the viewport and scissor rectangle
	{
		// Set up the viewport so we render into the correct
		// portion of the render target
		viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)Window::Width();
		viewport.Height = (float)Window::Height();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// Define a scissor rectangle that defines a portion of
		// the render target for clipping. This is different from
		// a viewport in that it is applied after the pixel shader.
		// We need at least one of these, but we're rendering to
		// the entire window, so it'll be the same size.
		scissorRect = {};
		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = Window::Width();
		scissorRect.bottom = Window::Height();
	}
}

void Game::CreateParticleRootSigPipelineState()
{
	// Blobs to hold raw shader byte code used in several steps below
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;

	// Load shaders
	{
		D3DReadFileToBlob(
			FixPath(L"VSParticle.cso").c_str(), vertexShaderByteCode.GetAddressOf());
		D3DReadFileToBlob(
			FixPath(L"PSParticle.cso").c_str(), pixelShaderByteCode.GetAddressOf());
	}

	// Input layout
	const unsigned int inputElementCount = 2;
	D3D12_INPUT_ELEMENT_DESC inputElements[inputElementCount] = {};
	{
		// position
		inputElements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // float3
		inputElements[0].SemanticName = "POSITION";            // must match semantic in shader
		inputElements[0].SemanticIndex = 0;                    // first POSITION semantic

		// uv
		inputElements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElements[1].SemanticName = "TEXCOORD";
		inputElements[1].SemanticIndex = 0;
	}

	// Root Signature
	{
		// Describe the range of CBVs needed for the vertex shader
		D3D12_DESCRIPTOR_RANGE cbvRangeVS = {};
		cbvRangeVS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangeVS.NumDescriptors = 1; // external data and structured buffer of particles
		cbvRangeVS.BaseShaderRegister = 0;
		cbvRangeVS.RegisterSpace = 0;
		cbvRangeVS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Describe the range of CBVs needed for the pixel shader
		D3D12_DESCRIPTOR_RANGE cbvRangePS = {};
		cbvRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangePS.NumDescriptors = 1;
		cbvRangePS.BaseShaderRegister = 0;
		cbvRangePS.RegisterSpace = 0;
		cbvRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create range of SRV's for vertex shader
		D3D12_DESCRIPTOR_RANGE srvRangeVS = {};
		srvRangeVS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRangeVS.NumDescriptors = 1; // Set to max number of textures at once (match pixel shader!)
		srvRangeVS.BaseShaderRegister = 0; // Starts at t0 (match pixel shader!)
		srvRangeVS.RegisterSpace = 0;
		srvRangeVS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create a range of SRV's for textures
		D3D12_DESCRIPTOR_RANGE srvRangePS = {};
		srvRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRangePS.NumDescriptors = 1; // Set to max number of textures at once (match pixel shader!)
		srvRangePS.BaseShaderRegister = 0; // Starts at t0 (match pixel shader!)
		srvRangePS.RegisterSpace = 0;
		srvRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create the root parameters
		D3D12_ROOT_PARAMETER rootParams[4] = {};

		// CBV table param for vertex shader
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[0].DescriptorTable.pDescriptorRanges = &cbvRangeVS;

		// CBV table param for pixel shader
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[1].DescriptorTable.pDescriptorRanges = &cbvRangePS;

		// SRV table param for vertex shader
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[2].DescriptorTable.pDescriptorRanges = &srvRangeVS;

		// SRV table param for pixel shader
		rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[3].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[3].DescriptorTable.pDescriptorRanges = &srvRangePS;

		// Create a single static sampler (available to all pixel shaders at the same slot)
		D3D12_STATIC_SAMPLER_DESC anisoWrap = {};
		anisoWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.Filter = D3D12_FILTER_ANISOTROPIC;
		anisoWrap.MaxAnisotropy = 16;
		anisoWrap.MaxLOD = D3D12_FLOAT32_MAX;
		anisoWrap.ShaderRegister = 0; // register(s0)
		anisoWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_STATIC_SAMPLER_DESC samplers[] = { anisoWrap };

		// Describe the full root signature
		D3D12_ROOT_SIGNATURE_DESC rootSig = {};
		rootSig.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSig.NumParameters = ARRAYSIZE(rootParams);
		rootSig.pParameters = rootParams;
		rootSig.NumStaticSamplers = ARRAYSIZE(samplers);
		rootSig.pStaticSamplers = samplers;

		ID3DBlob* serializedRootSig = 0;
		ID3DBlob* errors = 0;

		D3D12SerializeRootSignature(
			&rootSig,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSig,
			&errors);

		// Check for errors during serialization
		if (errors != 0)
		{
			OutputDebugString((wchar_t*)errors->GetBufferPointer());
		}

		// Actually create the root sig
		Graphics::Device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(particleRootSig.GetAddressOf()));
	}

	// Pipeline state
	{
		// Describe the pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		// -- Input assembler related ---
		psoDesc.InputLayout.NumElements = inputElementCount;
		psoDesc.InputLayout.pInputElementDescs = inputElements;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Overall primitive topology type (triangle, line, etc.) is set here
		// IASetPrimTop() is still used to set list/strip/adj options

		// Root sig
		psoDesc.pRootSignature = particleRootSig.Get();

		// -- Shaders (VS/PS) ---
		psoDesc.VS.pShaderBytecode = vertexShaderByteCode->GetBufferPointer();
		psoDesc.VS.BytecodeLength = vertexShaderByteCode->GetBufferSize();
		psoDesc.PS.pShaderBytecode = pixelShaderByteCode->GetBufferPointer();
		psoDesc.PS.BytecodeLength = pixelShaderByteCode->GetBufferSize();

		// -- Render targets ---
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;

		// -- States ---
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.DepthClipEnable = true;
		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;

		// -- Misc ---
		psoDesc.SampleMask = 0xffffffff;

		// Create the pipe state object
		Graphics::Device->CreateGraphicsPipelineState(
			&psoDesc,
			IID_PPV_ARGS(particlePipelineState.GetAddressOf()));
	}
}

// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Rezize camera projection matrix
	cam.UpdateProjectionMatrix((float)Window::Width(), (float)Window::Height());

	// Resize the viewport and scissor rectangle
	{
		viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)Window::Width();
		viewport.Height = (float)Window::Height();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// Remember we're rendering to entire window, so this
		// should still be the same size as the viewport.
		scissorRect = {};
		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = Window::Width();
		scissorRect.bottom = Window::Height();
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// rotate entities on their Y axes
	for (Entity& e : entities)
	{
		Transform t = e.GetWorldTM();
		t.Rotate(0, deltaTime, 0);
		e.SetWorldTM(t);
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

	// Clearing the render target
	{
		// Transition the back buffer from present to render target
		D3D12_RESOURCE_BARRIER rb = {};
		rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rb.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		rb.Transition.pResource = currentBackBuffer.Get();
		rb.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		rb.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Graphics::CommandList->ResourceBarrier(1, &rb);

		// Background color (Cornflower Blue in this case) for clearing
		float color[] = { 0.4f, 0.6f, 0.75f, 1.0f };

		// Clear the RTV
		Graphics::CommandList->ClearRenderTargetView(
			Graphics::RTVHandles[Graphics::SwapChainIndex()],
			color,
			0, 0); // No scissor rectangles

		// Clear the depth buffer, too
		Graphics::CommandList->ClearDepthStencilView(
			Graphics::DSVHandle,
			D3D12_CLEAR_FLAG_DEPTH,
			1.0f,                   // Max depth = 1.0f
			0,                      // Not clearing stencil, but need a value
			0, 0);                  // No scissor rects
	}

	// Rendering here!
	{
		// set descriptor heap for CBVs
		Graphics::CommandList->SetDescriptorHeaps(1, Graphics::cbvSrvDescriptorHeap.GetAddressOf());

		// Root sig (must happen before root descriptor table)
		Graphics::CommandList->SetGraphicsRootSignature(rootSignature.Get());

		// Set up other commands for rendering
		Graphics::CommandList->OMSetRenderTargets(
			1, &Graphics::RTVHandles[Graphics::SwapChainIndex()], true, &Graphics::DSVHandle);
		Graphics::CommandList->RSSetViewports(1, &viewport);
		Graphics::CommandList->RSSetScissorRects(1, &scissorRect);
		Graphics::CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		VSExternalData data = {};
		data.proj = cam.GetProjection();
		data.view = cam.GetView();

		// render entities
		for (Entity& e : entities)
		{
			std::shared_ptr<Mesh> mesh = e.GetMesh();
			std::shared_ptr<Material> mat = e.GetMaterial();

			// VS data
			{
				data.world = e.GetWorldTM().GetWorldMatrix();
				XMMATRIX worldM = XMLoadFloat4x4(&data.world);
				XMStoreFloat4x4(&data.worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldM)));

				// copy VS data into CB ring buffer and set root descriptor table to it
				D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle = Graphics::FillNextConstantBufferAndGetGPUDescriptorHandle(&data, sizeof(data));
				Graphics::CommandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
			}

			// PS data and cbuffer setup
			{
				PSExternalData psData = {};
				psData.uvScale = mat->GetUVScale();
				psData.uvOffset = mat->GetUVOffset();
				psData.cameraPosition = cam.GetTransform().GetPosition();
				psData.ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1);
				psData.lightCount = (unsigned int)lights.size();

				memcpy(psData.lights, &lights[0], sizeof(Light) * MAX_LIGHTS);

				// Send this to a chunk of the constant buffer heap
				// and grab the GPU handle for it so we can set it for this draw
				D3D12_GPU_DESCRIPTOR_HANDLE cbHandlePS =
					Graphics::FillNextConstantBufferAndGetGPUDescriptorHandle(
						(void*)(&psData), sizeof(PSExternalData));

				// Set this constant buffer handle
				// Note: This assumes that descriptor table 1 is the
				// place to put this particular descriptor. This
				// is based on how we set up our root signature.
				Graphics::CommandList->SetGraphicsRootDescriptorTable(1, cbHandlePS);
			}

			// Set overall pipeline state
			Graphics::CommandList->SetPipelineState(mat->GetPipelineState().Get());

			// Set the SRV descriptor handle for this material's textures
			// Note: This assumes that descriptor table 2 is for textures (as per our root sig)
			Graphics::CommandList->SetGraphicsRootDescriptorTable(2, mat->GetFinalGPUHandleForSRVs());

			// set VB and IB
			D3D12_VERTEX_BUFFER_VIEW vbView = mesh->GetVertexBufferView();
			D3D12_INDEX_BUFFER_VIEW ibView = mesh->GetIndexBufferView();
			Graphics::CommandList->IASetVertexBuffers(0, 1, &vbView);
			Graphics::CommandList->IASetIndexBuffer(&ibView);

			// draw
			Graphics::CommandList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
		}
	}

	// Present
	{
		// Transition back to present
		D3D12_RESOURCE_BARRIER rb = {};
		rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rb.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		rb.Transition.pResource = currentBackBuffer.Get();
		rb.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rb.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		Graphics::CommandList->ResourceBarrier(1, &rb);

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