//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <iostream>

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"
#include "Buffers.h"
#include "Cube.h"
#include "Engine/VertexLayout.h"


extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;


struct CameraData
{
	Matrix mView;
	Matrix mProjection;
};

//VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
//IndexBuffer indexBuffer;

ConstantBuffer<CameraData> cameraBuffer;

ComPtr<ID3D11InputLayout> inputLayout;
Cube cube;

float cameraDistance = 2.0f;
float cameraRotation = 0.0f;
float cameraSpeed = 1.f;


// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());

	auto device = m_deviceResources->GetD3DDevice();

	// TP: allouer vertexBuffer ici
	/*std::vector<float> triforceArray =
	{
		0, 0.6f, 0,
		0.3f, 0, 0,
		-0.3f, 0, 0,
		0, -0.6f, 0,
		-0.6f, -0.6f, 0,
		0.6f, -0.6f, 0,
	};*/

	GenerateInputLayout<VertexLayout_PositionUV>(m_deviceResources.get(), basicShader);
	
	/*std::vector<float> vertexArray =
	{
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};*/

	cube.Generate(Vector3(0,0,0));
	cube.Create(m_deviceResources.get());
	
	//vertexBuffer.PushVertex({{-0.5f, 0.5f, 0.0f, 0.0f}, {0.f,1.f}});
	//vertexBuffer.PushVertex({{0.5f, -0.5f, 0.0f, 0.0f}, {1.f, 0.f}});
	//vertexBuffer.PushVertex({{-0.5f, -0.5f, 0.0f, 0.0f}, {0.f, 0.f}});
	//vertexBuffer.PushVertex({{0.5f, 0.5f, 0.0f, 0.0f}, {1.f, 1.f}});
	//vertexBuffer.Create(m_deviceResources.get());
	
	/* CD3D11_BUFFER_DESC bufferDescVertex(vertexArray.size()*sizeof(float), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA subresourceDataVertex = {};
	subresourceDataVertex.pSysMem = vertexArray.data();
	device->CreateBuffer(&bufferDescVertex, &subresourceDataVertex, vertexBuffer.ReleaseAndGetAddressOf());*/

	/*std::vector<uint32_t> indexArray =
	{
		0, 1, 2,
		0, 3, 1,
	};*/
	
	//indexBuffer.PushTriangle(0,1,2);
	//indexBuffer.PushTriangle(0,3,1);
	//indexBuffer.Create(m_deviceResources.get());
	
	/*CD3D11_BUFFER_DESC bufferDescIndex(indexArray.size()*sizeof(uint32_t), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA subresourceDataIndex = {};
	subresourceDataIndex.pSysMem = indexArray.data();
	device->CreateBuffer(&bufferDescIndex, &subresourceDataIndex, indexBuffer.ReleaseAndGetAddressOf());*/

	// Matrix transformation
	cube.modelData.Create(m_deviceResources.get());
	cube.modelData.m_data.mModel = Matrix::CreateTranslation(Vector3(-0.5, -0.5, 0.5)).Transpose();
	//modelData.mModel = Matrix::Identity;
	
	/*CD3D11_BUFFER_DESC bufferDescModel(sizeof(CameraData), D3D11_BIND_CONSTANT_BUFFER);
	D3D11_SUBRESOURCE_DATA subresourceDataModel = {};
	subresourceDataModel.pSysMem = &modelData;
	device->CreateBuffer(&bufferDescModel, &subresourceDataModel, modelBuffer.ReleaseAndGetAddressOf());*/

	cameraBuffer.Create(m_deviceResources.get());
	CameraData cameraData;
	cameraData.mView = Matrix::CreateLookAt(Vector3(0, 0, cameraDistance), Vector3(0, 0, 0), Vector3::Up).Transpose();
	cameraData.mProjection = Matrix::CreatePerspectiveFieldOfView(90, static_cast<float>(width) / height, 0.1, 1000.0).Transpose();
	cameraBuffer.m_data = cameraData;
	
	/*cameraBuffer.Update(m_deviceResources.get(), )
		CD3D11_BUFFER_DESC bufferDescCamera(sizeof(CameraData), D3D11_BIND_CONSTANT_BUFFER);
	D3D11_SUBRESOURCE_DATA subresourceDataCamera = {};
	subresourceDataCamera.pSysMem = &cameraData;
	device->CreateBuffer(&bufferDescCamera, &subresourceDataCamera, cameraBuffer.ReleaseAndGetAddressOf());*/
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();

	cameraRotation += cameraSpeed*timer.GetElapsedSeconds();
	cameraBuffer.m_data.mView = Matrix::CreateLookAt(Vector3(cameraDistance*cos(cameraRotation), 0, cameraDistance*sin(cameraRotation)), Vector3(0, 0, 0), Vector3::Up).Transpose();
	
	// add kb/mouse interact here
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::Green);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	ApplyInputLayout<VertexLayout_PositionUV>(m_deviceResources.get());
	
	basicShader->Apply(m_deviceResources.get());

	// TP: Tracer votre vertex buffer ici
	/*ID3D11Buffer* vbs[] = { vertexBuffer.Get() };
	UINT strides[] = { sizeof(float) * 3 };
	UINT offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);*/

	//vertexBuffer.Apply(m_deviceResources.get());
	cube.Apply(m_deviceResources.get());
	//indexBuffer.Apply(m_deviceResources.get());

	cube.modelData.Update(m_deviceResources.get());
	cameraBuffer.Update(m_deviceResources.get());

	cube.modelData.ApplyToVS(m_deviceResources.get());
	cameraBuffer.ApplyToVS(m_deviceResources.get(), 1);
	/*m_deviceResources->GetD3DDeviceContext()->UpdateSubresource(cameraBuffer.Get(), 0, nullptr, &cameraData,0, 0);
	
	ID3D11Buffer* cbs[] = { modelBuffer.Get(), cameraBuffer.Get() };
	context->VSSetConstantBuffers(0, 2, cbs);*/

	
	context->DrawIndexed(cube.indexBuffer.Size(), 0, 0);
	
	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
	cameraBuffer.m_data.mProjection = Matrix::CreatePerspectiveFieldOfView(90, static_cast<float>(width) / height, 0.1, 1000.0).Transpose();
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion
