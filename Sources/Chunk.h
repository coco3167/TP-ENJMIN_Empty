#pragma once
#include "Block.h"
#include "Buffers.h"
#include "Engine/VertexLayout.h"

using namespace  DirectX::SimpleMath;


class Chunk
{
private:
    int faceIndex = 0;
    static constexpr int CHUNK_SIZE = 16;
    std::vector<std::vector<std::vector<BlockId>>> blockIds;

    inline bool IsCulled(const Vector3& idPos, const int& direction) const;
    
public:
    enum NeighborChunks
    {
        RIGHT,
        LEFT,
        FORWARD,
        BACKWARD,
        TOP,
        BOTTOM,
    };
    struct ModelData
    {
        Matrix mModel;
    };

    Vector3 m_position;
    std::vector<Chunk*> neighborChunks = std::vector<Chunk*>(6, nullptr);
    
    VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
    IndexBuffer indexBuffer;
    ConstantBuffer<ModelData> modelData;

    BlockId GetBlockId(const Vector3& idPosToCheck) const;

    void AddFace(const Vector3& pos, const Vector3& up, const Vector3& right, const int& texId);
    void PushCube(const Vector3& position, const Vector3& idPos);
    void GenerateBlockData(const float noiseValue);
    void GenerateCube(DeviceResources* deviceResources, const Vector3& basePosition);

    void Create(DeviceResources* deviceResources);
    void Apply(DeviceResources* deviceResources);
    void Draw(DeviceResources* deviceResources);
};
