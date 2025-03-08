#pragma once

#include <memory>

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

class Entity
{
public:

	Entity(std::shared_ptr<Mesh> model);
	Entity(std::shared_ptr<Mesh> model, std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh();
	const std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> material);

	Transform GetWorldTM();
	void SetWorldTM(Transform newTM);
	void SetWorldTM(DirectX::XMFLOAT3 newPos, DirectX::XMFLOAT3 newRot, DirectX::XMFLOAT3 newScale = DirectX::XMFLOAT3(1, 1, 1));

private:

	std::shared_ptr<Mesh> mesh;
	Transform tm;
	std::shared_ptr<Material> material;
};

