#include "pch.h"
#include "Chunk.h"

#include <iostream>

#include "Block.h"
#include "Buffers.h"
#include "World.h"




void Chunk::GenerateBlockData()
{
    boundingBox = DirectX::BoundingBox(m_position + Vector3(CHUNK_SIZE/2, CHUNK_SIZE/2, CHUNK_SIZE/2), Vector3(CHUNK_SIZE/2, CHUNK_SIZE/2, CHUNK_SIZE/2));

    for (int loop = 0; loop < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; ++loop)
    {
        Vector3 pos = IndexToCoordinate(loop);
        float noiseValue = World::perlinNoise.noise2D_01((m_position.x + pos.x)/20.f, (m_position.z + pos.z)/20.f);
        float height = noiseValue * World::HEIGHT_CHUNK*CHUNK_SIZE;
        float stoneHeight = (noiseValue*noiseValue/2 + noiseValue/5 - .1f)*World::HEIGHT_CHUNK*CHUNK_SIZE;
        float waterHeight = .4f*World::HEIGHT_CHUNK*CHUNK_SIZE;
        const float minNeige = .6f;
        const float fullNeige = .9f;
        float snowHeight = (minNeige + (fullNeige - minNeige)*noiseValue*noiseValue*noiseValue*noiseValue)*World::HEIGHT_CHUNK*CHUNK_SIZE;
        
        BlockId& blockId = blockIds.emplace_back();
        int realY = pos.y + m_position.y;
        if (realY>=height)
        {
            if (realY <= waterHeight)
                blockId = WATER;
            else
                blockId = EMPTY;
        }
        else if (realY>=height-1)
        {
            if (realY >= snowHeight)
                blockId = SNOW;
            else
                blockId = GRASS;
        }
        else if (realY < stoneHeight)
            blockId = STONE;
        else
            blockId = DIRT;
    }
}

void Chunk::GenerateCube(DeviceResources* deviceResources, const Vector3& basePosition)
{
    Vector3 position = CHUNK_SIZE*basePosition;    
    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z)
                PushCube(position + Vector3{float(x), float(y), float(z) }, Vector3(x,y,z));
    
    Create(deviceResources);
    modelData.m_data.mModel = Matrix::CreateTranslation(Vector3(-0.5, -0.5, 0.5)).Transpose();
    modelData.Create(deviceResources);
}

void Chunk::PushCube(const Vector3& position, const Vector3& idPos)
{
    int id = CoordinateToIndex(idPos);
    BlockId blockId = GetBlockId(id);
    if (blockId == EMPTY)
        return;
    BlockData blockData = BlockData::Get(blockId);

    if (IsCulled(idPos, FORWARD))
        AddFace(position, Vector3::Up, Vector3::Right, blockData.texIdSide, blockData.transparent);                                   //Forward
    if (IsCulled(idPos, RIGHT))
        AddFace(position + Vector3::Right, Vector3::Up, Vector3::Forward, blockData.texIdSide, blockData.transparent);           //Right
    if (IsCulled(idPos, LEFT))
        AddFace(position + Vector3::Forward, Vector3::Up, Vector3::Backward, blockData.texIdSide, blockData.transparent);        //Left
    if (IsCulled(idPos, BACKWARD))
        AddFace(position + Vector3(1,0,-1), Vector3::Up, Vector3::Left, blockData.texIdSide, blockData.transparent);    //Backward
    if (IsCulled(idPos, TOP))
        AddFace(position + Vector3::Up, Vector3::Forward, Vector3::Right, blockData.texIdTop, blockData.transparent);              //Top
    if (IsCulled(idPos, BOTTOM))
        AddFace(position + Vector3::Forward, Vector3::Backward, Vector3::Right, blockData.texIdBottom, blockData.transparent);     //Bottom
}

bool Chunk::IsCulled(const Vector3& idPos, const int& direction) const
{
    bool limitCondition;
    Vector3 idPosToCheck = idPos;
    int idIndex = CoordinateToIndex(idPosToCheck);
    
    switch (direction)
    {
        case FORWARD:
            idPosToCheck.z+=1;
            limitCondition = idPosToCheck.z > CHUNK_SIZE-1;
            break;
        
        case BACKWARD:
            idPosToCheck.z-=1;
            limitCondition = idPosToCheck.z < 0;
            break;
        
        case RIGHT:
            idPosToCheck.x+=1;
            limitCondition = idPosToCheck.x > CHUNK_SIZE-1;
            break;
        
        case LEFT:
            idPosToCheck.x-=1;
            limitCondition = idPosToCheck.x < 0;
            break;
        
        case TOP:
            idPosToCheck.y+=1;
            limitCondition = idPosToCheck.y > CHUNK_SIZE-1;
            break;
        
        case BOTTOM:
            idPosToCheck.y-=1;
            limitCondition = idPosToCheck.y < 0;
            break;
        
        default:
            return false;
    }
    if (limitCondition)
    {
        if (neighborChunks[direction] == nullptr)
            return true;
        idPosToCheck.x = static_cast<int>(idPosToCheck.x+CHUNK_SIZE)%CHUNK_SIZE;
        idPosToCheck.y = static_cast<int>(idPosToCheck.y+CHUNK_SIZE)%CHUNK_SIZE;
        idPosToCheck.z = static_cast<int>(idPosToCheck.z+CHUNK_SIZE)%CHUNK_SIZE;

        // Check transparency/Emptyness
        BlockId idToCheck = neighborChunks[direction]->GetBlockId(CoordinateToIndex(idPosToCheck));
        BlockId selfId = GetBlockId(idIndex);
        if (BlockData::Get(idToCheck).transparent)
        {
            if (BlockData::Get(selfId).transparent)
                return selfId != idToCheck;
            return true;
        }
        return idToCheck == EMPTY;
    }
    
    BlockId idToCheck = GetBlockId(CoordinateToIndex(idPosToCheck));
    BlockData blockData = BlockData::Get(idToCheck);
    return idToCheck == EMPTY || (blockData.transparent && idToCheck != GetBlockId(idIndex));
}

BlockId Chunk::GetBlockId(const int& index) const
{
    return blockIds[index];
}


Vector3 Chunk::IndexToCoordinate(const int& index) const
{
    int z = index % CHUNK_SIZE;
    int y = (index-z)/CHUNK_SIZE%CHUNK_SIZE;
    int x = (index-z-y*CHUNK_SIZE)/CHUNK_SIZE/CHUNK_SIZE;

    //std::cout << index << " | ";
    //std::cout << x << ", " << y << ", " << z << std::endl;
    
    return Vector3(x,y,z);
}

int Chunk::CoordinateToIndex(const Vector3& coordinate) const
{
    int index = coordinate.x*CHUNK_SIZE*CHUNK_SIZE + coordinate.y*CHUNK_SIZE + coordinate.z;
    //std::cout << coordinate.x << ", " << coordinate.y << ", " << coordinate.z << " | ";
    //std::cout << index << std::endl;
    return index;
}

void Chunk::AddFace(const Vector3& pos, const Vector3& up, const Vector3& right, const int &texId, bool isTransparent)
{
    float texIdX = texId%16;
    float texIdY = texId/16;
    auto& vertexBuffer = isTransparent ? vertexBufferAlpha : vertexBufferOpaque;
    auto& indexBuffer = isTransparent ? indexBufferAlpha : indexBufferOpaque;
    int& faceIndex = isTransparent ? faceIndexAlpha : faceIndexOpaque;
    
    vertexBuffer.PushVertex(
    {
        {pos},
        {texIdX + 1,texIdY + 1}
    });
    vertexBuffer.PushVertex(
    {
        {pos + up},
        {texIdX + 1,texIdY}
    });
    vertexBuffer.PushVertex(
    {
        {pos + right},
        {texIdX,texIdY + 1}
    });
    vertexBuffer.PushVertex(
    {
        {pos + up + right},
        {texIdX,texIdY}
    });
    
    indexBuffer.PushTriangle(faceIndex,faceIndex+1,faceIndex+2);
    indexBuffer.PushTriangle(faceIndex+1,faceIndex+3,faceIndex+2);
    faceIndex += 4;
}

void Chunk::Create(DeviceResources* deviceResources)
{
    vertexBufferOpaque.Create(deviceResources);
    indexBufferOpaque.Create(deviceResources);
    vertexBufferAlpha.Create(deviceResources);
    indexBufferAlpha.Create(deviceResources);
}

void Chunk::Apply(DeviceResources* deviceResources)
{
    vertexBufferOpaque.Apply(deviceResources);
    indexBufferOpaque.Apply(deviceResources);
    vertexBufferAlpha.Apply(deviceResources);
    indexBufferAlpha.Apply(deviceResources);
}

void Chunk::DrawOpaque(DeviceResources* deviceResources)
{
    if (indexBufferOpaque.Size() == 0)
        return;
    vertexBufferOpaque.Apply(deviceResources, 0);
    indexBufferOpaque.Apply(deviceResources);

    deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBufferOpaque.Size(), 0, 0);
}

void Chunk::DrawAlpha(DeviceResources* deviceResources)
{
    if (indexBufferAlpha.Size() == 0)
        return;
    vertexBufferAlpha.Apply(deviceResources, 0);
    indexBufferAlpha.Apply(deviceResources);

    deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBufferAlpha.Size(), 0, 0);
}
