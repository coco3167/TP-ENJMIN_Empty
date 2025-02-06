#include "pch.h"
#include "World.h"

#include <PerlinNoise.hpp>

void World::Generate(DeviceResources* deviceResources, int chunkNb)
{
    siv::BasicPerlinNoise<float> perlinNoise = siv::BasicPerlinNoise<float>();
    
    const int realChunkNb = chunkNb*chunkNb;
    for (int loop = 0; loop < realChunkNb; loop++)
    {
        float noiseValue = perlinNoise.noise2D_01(loop%chunkNb, loop/chunkNb);
        Chunk chunk;
        chunk.GenerateBlockData(noiseValue);
        m_chunks.push_back(chunk);
    }
    for (int loop = 0; loop < realChunkNb; loop++)
    {
        int x = loop%chunkNb;
        int z = loop/chunkNb;
        Chunk& chunk = m_chunks[loop];
        if (x != 0)
            chunk.neighborChunks[Chunk::LEFT] = &m_chunks[x-1+z*chunkNb];
        if (x != chunkNb-1)
            chunk.neighborChunks[Chunk::RIGHT] = &m_chunks[x+1+z*chunkNb];
        if (z != 0)
            chunk.neighborChunks[Chunk::BACKWARD] = &m_chunks[x+(z-1)*chunkNb];
        if (z != chunkNb-1)
            chunk.neighborChunks[Chunk::FORWARD] = &m_chunks[x+(z+1)*chunkNb];
        
        m_chunks[loop].GenerateCube(deviceResources, Vector3(x,0,z));
    }
}

void World::Render(DeviceResources* deviceResources)
{
    for (Chunk chunk : m_chunks)
    {
        chunk.modelData.Update(deviceResources);
        chunk.modelData.ApplyToVS(deviceResources, 0);
        chunk.Draw(deviceResources);
    }
}
