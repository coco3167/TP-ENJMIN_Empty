#pragma once
#include "Chunk.h"

class World
{
public:
    std::vector<Chunk> m_chunks;

    void Generate(DeviceResources* deviceResources, int chunkNb);
    void Render(DeviceResources* deviceResources);
};
