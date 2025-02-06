#include "pch.h"
#include "Chunk.h"

#include <iostream>

#include "Block.h"
#include "Buffers.h"


void Chunk::GenerateBlockData(const float noiseValue)
{
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        blockIds.push_back( std::vector<std::vector<BlockId>>(CHUNK_SIZE) );
        for (int y = 0; y < CHUNK_SIZE; ++y)
        {
            blockIds[x].push_back( std::vector<BlockId>(CHUNK_SIZE) );
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                BlockId blockId;
                if (y>6)
                    blockId = EMPTY;
                else
                    blockId = noiseValue>.5f ? DIRT : STONE;
                
                blockIds[x][y].push_back(blockId);
            }
        }
    }
}

void Chunk::GenerateCube(DeviceResources* deviceResources, const Vector3& basePosition)
{
    m_position = 16*basePosition;    
    for (int x = 0; x < CHUNK_SIZE; ++x)
        for (int y = 0; y < CHUNK_SIZE; ++y)
            for (int z = 0; z < CHUNK_SIZE; ++z)
                PushCube(m_position + Vector3{x * 1.0f, y * 1.0f, z * 1.0f }, Vector3(x,y,z));
    
    Create(deviceResources);
    modelData.Create(deviceResources);
    modelData.m_data.mModel = Matrix::CreateTranslation(Vector3(-0.5, -0.5, 0.5)).Transpose();
}

void Chunk::PushCube(const Vector3& position, const Vector3& idPos)
{
    BlockId blockId = GetBlockId(idPos);
    if (blockId == EMPTY)
        return;
    BlockData blockData = BlockData::Get(blockId);

    if (IsCulled(idPos, FORWARD))
        AddFace(position, Vector3::Up, Vector3::Right, blockData.texIdSide);                                   //Forward
    if (IsCulled(idPos, RIGHT))
        AddFace(position + Vector3::Right, Vector3::Up, Vector3::Forward, blockData.texIdSide);           //Right
    if (IsCulled(idPos, LEFT))
        AddFace(position + Vector3::Forward, Vector3::Up, Vector3::Backward, blockData.texIdSide);        //Left
    if (IsCulled(idPos, BACKWARD))
        AddFace(position + Vector3(1,0,-1), Vector3::Up, Vector3::Left, blockData.texIdSide);    //Backward
    if (IsCulled(idPos, TOP))
        AddFace(position + Vector3::Up, Vector3::Forward, Vector3::Right, blockData.texIdTop);              //Top
    if (IsCulled(idPos, BOTTOM))
        AddFace(position + Vector3::Forward, Vector3::Backward, Vector3::Right, blockData.texIdBottom);     //Bottom
}

inline bool Chunk::IsCulled(const Vector3& idPos, const int& direction) const
{
    int limitParameter;
    int limitValue;
    Vector3 idPosToCheck = idPos;
    Vector3 idPosOtherChunk = idPos;
    switch (direction)
    {
        case FORWARD:
            idPosToCheck.z+=1;
            idPosOtherChunk.z = 0;
            limitValue = CHUNK_SIZE-1;
            limitParameter = idPos.z;
            break;
        case BACKWARD:
            idPosToCheck.z-=1;
            idPosOtherChunk.z = CHUNK_SIZE-1;
            limitValue = 0;
            limitParameter = idPos.z;
            break;
        case RIGHT:
            idPosToCheck.x+=1;
            idPosOtherChunk.x = 0;
            limitValue = CHUNK_SIZE-1;
            limitParameter = idPos.x;
            break;
        case LEFT:
            idPosToCheck.x-=1;
            idPosOtherChunk.x = CHUNK_SIZE-1;
            limitValue = 0;
            limitParameter = idPos.x;
            break;
        case TOP:
            idPosToCheck.y+=1;
            idPosOtherChunk.y = 0;
            limitValue = CHUNK_SIZE-1;
            limitParameter = idPos.y;
            break;
        case BOTTOM:
            idPosToCheck.y-=1;
            idPosOtherChunk.y = CHUNK_SIZE-1;
            limitValue = 0;
            limitParameter = idPos.y;
            break;
        default:
            return false;
    }
    if (limitParameter == limitValue)
    {
        if (neighborChunks[direction] == nullptr)
            return true;
        return neighborChunks[direction]->GetBlockId(idPosOtherChunk) == EMPTY;
    }
    BlockId idToCheck = GetBlockId(idPosToCheck);
    BlockData blockData = BlockData::Get(idToCheck);
    return idToCheck == EMPTY || (blockData.transparent && idToCheck != GetBlockId(idPos));
}

BlockId Chunk::GetBlockId(const Vector3& idPosToCheck) const
{
    return blockIds[idPosToCheck.x][idPosToCheck.y][idPosToCheck.z];
}

void Chunk::AddFace(const Vector3& pos, const Vector3& up, const Vector3& right, const int &texId)
{
    float texIdX = texId%16;
    float texIdY = texId/16;
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
    vertexBuffer.Create(deviceResources);
    indexBuffer.Create(deviceResources);
}

void Chunk::Apply(DeviceResources* deviceResources)
{
    vertexBuffer.Apply(deviceResources);
    indexBuffer.Apply(deviceResources);
}

void Chunk::Draw(DeviceResources* deviceResources)
{
    vertexBuffer.Apply(deviceResources, 0);
    indexBuffer.Apply(deviceResources);

    deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBuffer.Size(), 0, 0);
}
