#include "pch.h"
#include "Cube.h"
#include "Buffers.h"


Cube::Cube(ID3D11Device1* device) : device(device)
{
}

Cube::~Cube()
{
    delete device;
}

void Cube::AddFace(const Vector3& pos, const Vector3& up, const Vector3& right)
{
    vertexBuffers.push_back(Microsoft::WRL::ComPtr<ID3D11Buffer>());
    std::vector<float> positions =
    {
        pos.x, pos.y, pos.z,
        pos.x + up.x, pos.y + up.y, pos.z + up.z,
        pos.x + right.x, pos.y + right.y, pos.z + right.z,
        pos.x + up.x + right.x, pos.y + up.y + right.y, pos.z + up.z + right.z,
    };
    //Buffers::CreateVertexBuffer(positions, device, vertexBuffers.front());
}
