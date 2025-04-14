#include "Camera.h"

#include <iostream>

#define PI 3.14159265f

using namespace DirectX;
using namespace std;

Camera::Camera(CamType camType,
	float viewWidth, float viewHeight,
	float fov, float nearClip, float farClip,
	XMFLOAT3 position, XMFLOAT3 rotation,
	float movementSpeed, float mouseSens)
{
	this->camType = camType;
	transform = Transform();
	transform.SetPosition(position);
	transform.SetRotation(rotation);
	moveSpeed = movementSpeed;
	sensitivity = mouseSens;
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->viewDimensions = { viewWidth, viewHeight };

	UpdateProjectionMatrix(viewWidth, viewHeight);
	UpdateViewMatrix();
}

Camera::Camera(Camera& other)
{
	this->camType = other.camType;
	transform = other.transform;
	transform.SetPosition(other.transform.GetPosition());
	transform.SetRotation(other.transform.GetPitchYawRoll());
	moveSpeed = other.moveSpeed;
	sensitivity = other.sensitivity;
	this->fov = other.fov;
	this->nearClip = other.nearClip;
	this->farClip = other.farClip;
	this->viewDimensions = other.viewDimensions;

	UpdateProjectionMatrix(viewDimensions.x, viewDimensions.y);
	UpdateViewMatrix();
}

void Camera::Update(float dt)
{
	if (Input::KeyDown('W')) { transform.MoveRelative(0, 0, dt * moveSpeed); }  // FORWARD
	if (Input::KeyDown('S')) { transform.MoveRelative(0, 0, -dt * moveSpeed); } // BACKWARD
	if (Input::KeyDown('A')) { transform.MoveRelative(-dt * moveSpeed, 0, 0); } // LEFT
	if (Input::KeyDown('D')) { transform.MoveRelative(dt * moveSpeed, 0, 0); }  // RIGHT
	if (Input::KeyDown('E')) { transform.MoveAbsolute(0, dt * moveSpeed, 0); }  // UP
	if (Input::KeyDown('Q')) { transform.MoveAbsolute(0, -dt * moveSpeed, 0); } // DOWN

	if (Input::MouseRightDown())
	{
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();

		transform.SetRotation(transform.GetPitchYawRoll().x + ((float)cursorMovementY * sensitivity / 100.0f),
			transform.GetPitchYawRoll().y + (float)cursorMovementX * sensitivity / 100.0f,
			transform.GetPitchYawRoll().z);

		// Clamp Pitch to +-PI/2 (slightly less than that)
		XMVECTOR pitchVec = XMVectorClamp(XMLoadFloat(&transform.GetPitchYawRoll().x),
			XMVectorSet(-PI / 2.01f, 0, 0, 0), XMVectorSet(PI / 2.01f, 0, 0, 0));
		XMStoreFloat(&transform.GetPitchYawRoll().x, pitchVec);
	}

	UpdateViewMatrix();
}

// Updates the camera's projection matrix
void Camera::UpdateProjectionMatrix(float viewWidth, float viewHeight)
{
	this->viewDimensions = { viewWidth, viewHeight };
	XMStoreFloat4x4(&projection, camType == Perspective ?
		XMMatrixPerspectiveFovLH(fov * (PI / 180.0f), viewWidth / viewHeight, nearClip, farClip) :
		XMMatrixOrthographicLH(viewWidth, viewHeight, nearClip, farClip));
}

// Update the camera's view matrix
void Camera::UpdateViewMatrix()
{
	XMStoreFloat4x4(&view, XMMatrixLookToLH(
		XMLoadFloat3(&transform.GetPosition()), // position
		XMLoadFloat3(&transform.GetForward()),  // look direction
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));  // global up
}

// Matrix Getters
XMFLOAT4X4 Camera::GetView()
{
	return view;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return projection;
}

Transform& Camera::GetTransform()
{
	return transform;
}