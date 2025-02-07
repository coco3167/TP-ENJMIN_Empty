#pragma once
#include <PerlinNoise.hpp>

#include "Camera.h"
#include "Chunk.h"

class World
{
public:
    static constexpr int NB_CHUNK = 8;
    static constexpr int HEIGHT_CHUNK = 2;

    std::vector<Chunk> m_chunks;
    static siv::BasicPerlinNoise<float> perlinNoise;

    void Generate(DeviceResources* deviceResources);
    void RenderOpaque(DeviceResources* deviceResources, const Camera& camera);
    void RenderAlpha(DeviceResources* deviceResources, const Camera& camera);

    int CoordinateToIndex(const Vector3& coordinate) const;
    Vector3 IndexToCoordinate(const int& index) const;
};
