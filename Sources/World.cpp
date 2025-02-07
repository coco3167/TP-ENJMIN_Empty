#include "pch.h"
#include "World.h"

#include <iostream>
#include <PerlinNoise.hpp>

#include "Camera.h"

siv::BasicPerlinNoise<float> World::perlinNoise = siv::BasicPerlinNoise<float>(); 

void World::Generate(DeviceResources* deviceResources)
{
    for (int loop = 0; loop < NB_CHUNK*NB_CHUNK*HEIGHT_CHUNK; loop++)
    {
        Vector3 coordinate = IndexToCoordinate(loop);
        Chunk& chunk = m_chunks.emplace_back();
        chunk.m_position = Vector3(coordinate.x*Chunk::CHUNK_SIZE, coordinate.y*Chunk::CHUNK_SIZE, coordinate.z*Chunk::CHUNK_SIZE);
        chunk.GenerateBlockData();
    }
    
    for (int loop = 0; loop < NB_CHUNK*NB_CHUNK*HEIGHT_CHUNK; loop++)
    {
        Vector3 coordinate = IndexToCoordinate(loop);
        Chunk& chunk = m_chunks[loop];
        if (coordinate.x > 0)
            chunk.neighborChunks[Chunk::LEFT] = &m_chunks[CoordinateToIndex(coordinate + Vector3(-1,0,0))];
        if (coordinate.x < NB_CHUNK-1)
            chunk.neighborChunks[Chunk::RIGHT] = &m_chunks[CoordinateToIndex(coordinate + Vector3(1,0,0))];
        if (coordinate.z > 0)
            chunk.neighborChunks[Chunk::BACKWARD] = &m_chunks[CoordinateToIndex(coordinate + Vector3(0,0,-1))];
        if (coordinate.z < NB_CHUNK-1)
            chunk.neighborChunks[Chunk::FORWARD] = &m_chunks[CoordinateToIndex(coordinate + Vector3(0,0,1))];
        if (coordinate.y > 0)
            chunk.neighborChunks[Chunk::BOTTOM] = &m_chunks[CoordinateToIndex(coordinate + Vector3(0,-1,0))];
        if (coordinate.y < HEIGHT_CHUNK-1)
            chunk.neighborChunks[Chunk::TOP] = &m_chunks[CoordinateToIndex(coordinate + Vector3(0,1,0))];
        
        m_chunks[loop].GenerateCube(deviceResources, Vector3(coordinate.x,coordinate.y,coordinate.z));
    }
}

Vector3 World::IndexToCoordinate(const int& index) const
{
    int z = index % NB_CHUNK;
    int y = (index-z)/NB_CHUNK%HEIGHT_CHUNK;
    int x = (index-z-y*NB_CHUNK)/NB_CHUNK/HEIGHT_CHUNK;

    //std::cout << index << " | ";
    //std::cout << x << ", " << y << ", " << z << std::endl;
    
    return Vector3(x,y,z);
}

int World::CoordinateToIndex(const Vector3& coordinate) const
{
    int index = coordinate.x*NB_CHUNK*HEIGHT_CHUNK + coordinate.y*NB_CHUNK + coordinate.z;
    //std::cout << coordinate.x << ", " << coordinate.y << ", " << coordinate.z << " | ";
    //std::cout << index << std::endl;
    return index;
}

void World::RenderOpaque(DeviceResources* deviceResources, const Camera& camera)
{
    for (Chunk chunk : m_chunks)
    {
        chunk.modelData.Update(deviceResources);
        chunk.modelData.ApplyToVS(deviceResources, 0);
        if (camera.m_boundingFrustrum.Intersects(chunk.boundingBox))
            chunk.DrawOpaque(deviceResources);
    }
}

void World::RenderAlpha(DeviceResources* deviceResources, const Camera& camera)
{
    for (Chunk chunk : m_chunks)
    {
        chunk.modelData.Update(deviceResources);
        chunk.modelData.ApplyToVS(deviceResources, 0);
        if (camera.m_boundingFrustrum.Intersects(chunk.boundingBox))
            chunk.DrawAlpha(deviceResources);
    }
}