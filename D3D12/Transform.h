#pragma once

#include <DirectXMath.h>

class Transform
{
private:

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;

	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 forward;

	void UpdateMatrices();
	void UpdateLocalAxes();

public:

	static DirectX::XMFLOAT3 WorldRight;
	static DirectX::XMFLOAT3 WorldUp;
	static DirectX::XMFLOAT3 WorldForward;

	Transform();
	Transform(
		DirectX::XMFLOAT3 position, 
		DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0, 0, 0), 
		DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1, 1, 1));

	// Transformation methods
	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	// Setters
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	// Getters
	DirectX::XMFLOAT3& GetPosition();
	DirectX::XMFLOAT3& GetPitchYawRoll();
	DirectX::XMFLOAT3& GetScale();

	DirectX::XMFLOAT3& GetRight();
	DirectX::XMFLOAT3& GetUp();
	DirectX::XMFLOAT3& GetForward();

	DirectX::XMFLOAT4X4& GetWorldMatrix();
	DirectX::XMFLOAT4X4& GetWorldInverseTransposeMatrix();
};