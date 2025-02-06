#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : fov(fov) {
	// TP initialiser matrices
	view = Matrix::CreateLookAt(camPos, Vector3::Forward, Vector3::Up);
	//view = Matrix::CreateFromQuaternion(camRot);
	//view.Translation(camPos);
	projection = Matrix::CreatePerspectiveFieldOfView(fov*XM_PI/180, aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}

void Camera::UpdateAspectRatio(float aspectRatio) {
	projection = Matrix::CreatePerspectiveFieldOfView(fov*XM_PI/180, aspectRatio, 0.1, 1000.0);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse) {
	float camSpeedRot = 0.25f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix im = view.Invert();

	// TP: deplacement par clavier
	if (kb.Z)
	{
		camPos += Vector3::TransformNormal(Vector3::Forward, im)*dt*camSpeed;
	}
	if (kb.Q)
	{
		camPos += Vector3::TransformNormal(Vector3::Left, im)*dt*camSpeed;
	}
	if (kb.D)
	{
		camPos += Vector3::TransformNormal(Vector3::Right, im)*dt*camSpeed;
	}
	if (kb.S)
	{
		camPos += Vector3::TransformNormal(Vector3::Backward, im)*dt*camSpeed;
	}

	// astuce: Vector3::TransformNormal(vecteur, im); transforme un vecteur de l'espace cameravers l'espace monde

	float zoomVertical = 0.f;
	float zoomHorizontal = 0.f;
	
    if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;
        if (mstate.rightButton) { 
			// TP Translate camera a partir de dx/dy
        	zoomVertical += -dy * dt * camSpeedMouse;
        	zoomHorizontal += dx * dt * camSpeedMouse;

        } else if (mstate.leftButton) {
			// TP Rotate camera a partir de dx/dy
        	camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -dx*dt*camSpeedRot);
        	camRot *= Quaternion::CreateFromAxisAngle(Vector3::TransformNormal(Vector3::Right, im), -dy*dt*camSpeedRot);
        } else {
            mouse->SetMode(Mouse::MODE_ABSOLUTE);
        }
    } else if (mstate.rightButton || mstate.leftButton) {
        mouse->SetMode(Mouse::MODE_RELATIVE);
    }

	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);
	Vector3 newRight = Vector3::Transform(Vector3::Right, camRot);

	camPos += newForward*zoomVertical;
	camPos += newRight*zoomHorizontal;

	// TP updater matrice view
	view = Matrix::CreateLookAt(camPos, camPos+newForward, newUp);
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!cbCamera) {
		cbCamera = new ConstantBuffer<MatrixData>();
		cbCamera->Create(deviceRes);
	}
	cbCamera->m_data = MatrixData();
	cbCamera->m_data.mProjection = projection.Transpose();
	cbCamera->m_data.mView = view.Transpose();
	cbCamera->Update(deviceRes);
	cbCamera->ApplyToVS(deviceRes, 1);

	// TP envoyer data
}
