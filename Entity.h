#pragma once

#include <memory>

#include "Mesh.h"
#include "Transform.h"

class Entity
{
public:

	Entity(std::shared_ptr<Mesh> model);

	std::shared_ptr<Mesh> GetMesh();

	Transform GetWorldTM();
	void SetWorldTM(Transform newTM);

private:

	std::shared_ptr<Mesh> mesh;
	Transform tm;

};

