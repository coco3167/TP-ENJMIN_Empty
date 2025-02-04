#include "pch.h"
#include "Cube.h"
#include "Buffers.h"

void Cube::Generate(const Vector3& position)
{
    AddFace(position, Vector3::Up, Vector3::Right);
    AddFace(position + Vector3::Right, Vector3::Up, Vector3::Forward);
    AddFace(position + Vector3::Forward, Vector3::Up, Vector3::Backward);
    AddFace(position + Vector3(1,0,-1), Vector3::Up, Vector3::Left);
    AddFace(position + Vector3::Up, Vector3::Forward, Vector3::Right);
    AddFace(position + Vector3::Forward, Vector3::Backward, Vector3::Right);
}

void Cube::AddFace(const Vector3& pos, const Vector3& up, const Vector3& right)
{
    vertexBuffer.PushVertex({{pos}, {0,0}});
    vertexBuffer.PushVertex({{pos + up}, {0,1}});
    vertexBuffer.PushVertex({{pos + right}, {1,0}});
    vertexBuffer.PushVertex({{pos + up + right}, {1,1}});
    indexBuffer.PushTriangle(faceIndex,faceIndex+1,faceIndex+2);
    indexBuffer.PushTriangle(faceIndex+1,faceIndex+3,faceIndex+2);
    faceIndex += 4;
}

void Cube::Create(DeviceResources* deviceResources)
{
    vertexBuffer.Create(deviceResources);
    indexBuffer.Create(deviceResources);
}

void Cube::Apply(DeviceResources* deviceResources)
{
    vertexBuffer.Apply(deviceResources);
    indexBuffer.Apply(deviceResources);
}
