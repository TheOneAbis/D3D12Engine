#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> model)
{
	mesh = model;
}

Entity::Entity(std::shared_ptr<Mesh> model, std::shared_ptr<Material> material)
{
	mesh = model;
	this->material = material;
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

Transform Entity::GetWorldTM()
{
	return tm;
}

void Entity::SetWorldTM(Transform newTM)
{
	tm = newTM;
}
void Entity::SetWorldTM(DirectX::XMFLOAT3 newPos, DirectX::XMFLOAT3 newRot, DirectX::XMFLOAT3 newScale)
{
	tm = Transform(newPos, newRot, newScale);
}

const std::shared_ptr<Material> Entity::GetMaterial()
{
	return material;
}

void Entity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}