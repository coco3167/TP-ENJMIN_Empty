#pragma once
#include "Block.h"
#include "Buffers.h"
#include "Engine/VertexLayout.h"

using namespace  DirectX::SimpleMath;


class Chunk
{
private:
    int faceIndexOpaque = 0;
    int faceIndexAlpha = 0;
    
    std::vector<BlockId> blockIds;
    bool IsCulled(const Vector3& idPos, const int& direction) const;
    Vector3 IndexToCoordinate(const int& index) const;
    int CoordinateToIndex(const Vector3& coordinate) const;

public:
    static constexpr int CHUNK_SIZE = 16;
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
    DirectX::BoundingBox boundingBox;
    
    VertexBuffer<VertexLayout_PositionUV> vertexBufferOpaque, vertexBufferAlpha;
    IndexBuffer indexBufferOpaque, indexBufferAlpha;
    ConstantBuffer<ModelData> modelData;

    BlockId GetBlockId(const int& index) const;
    
    void AddFace(const Vector3& pos, const Vector3& up, const Vector3& right, const int& texId, bool isTransparent);
    void PushCube(const Vector3& position, const Vector3& idPos);
    void GenerateBlockData();
    void GenerateCube(DeviceResources* deviceResources, const Vector3& basePosition);

    void Create(DeviceResources* deviceResources);
    void Apply(DeviceResources* deviceResources);
    void DrawOpaque(DeviceResources* deviceResources);
    void DrawAlpha(DeviceResources* deviceResources);
};