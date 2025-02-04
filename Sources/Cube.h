#pragma once

using namespace  DirectX::SimpleMath;


class Cube
{
private:
    ID3D11Device1* device;
public:
    Cube(ID3D11Device1* device);
    ~Cube();
    
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vertexBuffers;

    void AddFace(const Vector3& pos, const Vector3& up, const Vector3& right);
};
