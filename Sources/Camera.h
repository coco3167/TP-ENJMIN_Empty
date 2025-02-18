#pragma once

#include "Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera {
	float fov;
	float nearPlane = 0.01f;
	float farPlane = 500.0f;
	
	Vector3 camPos = Vector3(0, 20, 0);
	Quaternion camRot = Quaternion();
	Matrix projection;
	Matrix view;
	float time = 0.0f;

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
		float time;
		Vector3 padding;
	};
	ConstantBuffer<MatrixData>* cbCamera = nullptr;
public:
	BoundingFrustum m_boundingFrustrum;
	
	Camera(float fov, float aspectRatio);
	~Camera();

	void UpdateAspectRatio(float aspectRatio);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse* ms);

	void ApplyCamera(DeviceResources* deviceRes);
};