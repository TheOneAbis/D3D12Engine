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

private:

	std::shared_ptr<Mesh> mesh;
	Transform tm;
	std::shared_ptr<Material> material;
};

