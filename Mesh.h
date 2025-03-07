#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include "Vertex.h"

class Mesh
{
private:

	// Vertex and index buffers, context pointer
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW ibView{};

protected:

	// Hold num indices in index buffer
	int indexCount;
	std::vector<Vertex> vertices;

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	void CreateBuffers(unsigned int* indices, unsigned int numIndices);

public:

	Mesh();

	Mesh(Vertex* vertices,
		unsigned int numVerts,
		unsigned int* indices,
		unsigned int numIndices, bool dynamic = false);

	Mesh(const wchar_t* fileName, bool dynamic = false);

	~Mesh();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer();
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
	unsigned int GetIndexCount();
	unsigned int GetVertexCount();

};