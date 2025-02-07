#include "Transform.h"

using namespace DirectX;

XMFLOAT3 Transform::WorldRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
XMFLOAT3 Transform::WorldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
XMFLOAT3 Transform::WorldForward = XMFLOAT3(0.0f, 0.0f, 1.0f);

Transform::Transform()
{
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);

	forward = XMFLOAT3(0, 0, 1);
	up = XMFLOAT3(0, 1, 0);
	right = XMFLOAT3(1, 0, 0);

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

Transform::Transform(
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 rotation,
	DirectX::XMFLOAT3 scale)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;

	UpdateLocalAxes();
	UpdateMatrices();
}

void Transform::UpdateMatrices()
{
	// Translation matrix
	XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);

	// Rotation matrix (from quaternion)
	XMVECTOR quatVec = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX r = XMMatrixRotationQuaternion(quatVec);

	// Scale matrix
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(s, r), t);
	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(world)));
}

// Transformation methods
void Transform::MoveAbsolute(float x, float y, float z)
{
	XMVECTOR posVec = XMVectorAdd(XMLoadFloat3(&position), XMVectorSet(x, y, z, 0.0f));
	XMStoreFloat3(&position, posVec);
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	XMVECTOR posVec = XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&offset));
	XMStoreFloat3(&position, posVec);
}

void Transform::MoveRelative(float x, float y, float z)
{
	// Create the movement vector rotated by transform's current rotation Quat
	XMVECTOR rotVec = XMVector3Rotate(XMVectorSet(x, y, z, 0.0f),
		XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));

	// Add the rotated movement vector to transform's current position
	XMStoreFloat3(&position,
		XMVectorAdd(XMLoadFloat3(&position), rotVec));
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	XMVECTOR rotVec = XMVectorAdd(XMLoadFloat3(&this->rotation), XMVectorSet(pitch, yaw, roll, 0.0f));
	XMStoreFloat3(&this->rotation, rotVec);
	UpdateLocalAxes();
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	XMVECTOR rotVec = XMVectorAdd(XMLoadFloat3(&this->rotation), XMLoadFloat3(&rotation));
	XMStoreFloat3(&this->rotation, rotVec);
	UpdateLocalAxes();
}

void Transform::Scale(float x, float y, float z)
{
	XMVECTOR scaleVec = XMVectorAdd(XMLoadFloat3(&scale), XMVectorSet(x, y, z, 0.0f));
	XMStoreFloat3(&scale, scaleVec);
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	XMVECTOR scaleVec = XMVectorAdd(XMLoadFloat3(&this->scale), XMLoadFloat3(&scale));
	XMStoreFloat3(&this->scale, scaleVec);
}

// Setters
void Transform::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	SetRotation(XMFLOAT3(pitch, yaw, roll));
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation = rotation;
	UpdateLocalAxes();
}

void Transform::SetScale(float x, float y, float z)
{
	SetScale(XMFLOAT3(x, y, z));
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
}

// Get Position, Rotation, and Scale
DirectX::XMFLOAT3& Transform::GetPosition()
{
	return this->position;
}

DirectX::XMFLOAT3& Transform::GetPitchYawRoll()
{
	return this->rotation;
}

DirectX::XMFLOAT3& Transform::GetScale()
{
	return this->scale;
}

// Get Local Right, Up, and Forward
DirectX::XMFLOAT3& Transform::GetRight()
{
	return this->right;
}

DirectX::XMFLOAT3& Transform::GetUp()
{
	return this->up;
}

DirectX::XMFLOAT3& Transform::GetForward()
{
	return this->forward;
}

// Get World and Inverse Transpose Matrices
DirectX::XMFLOAT4X4& Transform::GetWorldMatrix()
{
	UpdateMatrices();
	return worldMatrix;
}

DirectX::XMFLOAT4X4& Transform::GetWorldInverseTransposeMatrix()
{
	UpdateMatrices();
	return worldInverseTranspose;
}

// Update transform's local right, up and forward axes
void Transform::UpdateLocalAxes()
{
	// Create the rotation quat
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	// Rotate local right
	XMStoreFloat3(&right, XMVector3Rotate(XMLoadFloat3(&WorldRight),
		rotQuat));

	// Rotate local up
	XMStoreFloat3(&up, XMVector3Rotate(XMLoadFloat3(&WorldUp),
		rotQuat));

	// Rotate local forward
	XMStoreFloat3(&forward, XMVector3Rotate(XMLoadFloat3(&WorldForward),
		rotQuat));
}