#pragma once

#include "Input.h"
#include "Transform.h"

enum CamType
{
	Perspective,
	Orthographic
};

class Camera
{
public:

	// Camera's Field of View.
	float fov;
	// Camera's near and far clip plane depth values.
	float nearClip, farClip;
	float moveSpeed;
	float sensitivity;
	// The camera's view width (x) and height (y).
	DirectX::XMFLOAT2 viewDimensions;
	CamType camType;

	Camera(CamType camType = Perspective,
		float viewWidth = 1280.0f,
		float viewHeight = 720.0f,
		float fov = 80.0f,
		float nearClip = 0.1f,
		float farClip = 1000.0f,
		DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0, 0, 0),
		float movementSpeed = 2.0f,
		float mouseSens = 0.5f);

	Camera(Camera& other);

	void Update(float dt);

	Transform& GetTransform();
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	void UpdateProjectionMatrix(float viewWidth, float viewHeight);
	void UpdateViewMatrix();

private:

	Transform transform;
	DirectX::XMFLOAT4X4 view, projection;
};