#pragma once
#include "Buffers.h"
#include "Engine/VertexLayout.h"

using namespace  DirectX::SimpleMath;


class Cube
{
private:
    int faceIndex = 0;
public:
    VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
    IndexBuffer indexBuffer;

    void Generate(const Vector3& position);
    void AddFace(const Vector3& pos, const Vector3& up, const Vector3& right);
    void Create(DeviceResources* deviceResources);
    void Apply(DeviceResources* deviceResources);
};
