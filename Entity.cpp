#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> model)
{
	mesh = model;
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