#pragma once

template<typename TVertex>
class VertexBuffer
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    std::vector<TVertex> data;
public:
    VertexBuffer() {};
    uint32_t PushVertex(TVertex vertex)
    {
        data.push_back(vertex);
        return data.size() - 1;
    };
    
    
    void Create(DeviceResources* deviceResources)
    {
        CD3D11_BUFFER_DESC bufferDescVertex(data.size()*sizeof(TVertex), D3D11_BIND_VERTEX_BUFFER);
        D3D11_SUBRESOURCE_DATA subresourceDataVertex = {};
        subresourceDataVertex.pSysMem = data.data();
        deviceResources->GetD3DDevice()->CreateBuffer(&bufferDescVertex, &subresourceDataVertex, m_buffer.ReleaseAndGetAddressOf());
    };

    void Apply(DeviceResources* deviceResources, int slot = 0)
    {
        ID3D11Buffer* vbs[] = { m_buffer.Get() };
        UINT strides[] = { sizeof(TVertex) };
        UINT offsets[] = { 0 };
        deviceResources->GetD3DDeviceContext()->IASetVertexBuffers(slot, 1, vbs, strides, offsets);
    }
};

class IndexBuffer
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    std::vector<uint32_t> m_indices;
public:
    IndexBuffer() {};
    void PushTriangle(uint32_t a, uint32_t b, uint32_t c)
    {
        m_indices.push_back(a);
        m_indices.push_back(b);
        m_indices.push_back(c);
    }

    size_t Size()
    {
        return m_indices.size();
    }

    void Create(DeviceResources* deviceResources)
    {
        CD3D11_BUFFER_DESC bufferDescIndex(Size()*sizeof(uint32_t), D3D11_BIND_INDEX_BUFFER);
        D3D11_SUBRESOURCE_DATA subresourceDataIndex = {};
        subresourceDataIndex.pSysMem = m_indices.data();
        deviceResources->GetD3DDevice()->CreateBuffer(&bufferDescIndex, &subresourceDataIndex, m_buffer.ReleaseAndGetAddressOf());
    }

    void Apply(DeviceResources* deviceResources, int offset = 0)
    {
        deviceResources->GetD3DDeviceContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);
    }
};

template<typename TData>
class ConstantBuffer
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    public:
    TData m_data;
    ConstantBuffer() {};

    void Create(DeviceResources* deviceResources)
    {
        CD3D11_BUFFER_DESC bufferDescModel(sizeof(TData), D3D11_BIND_CONSTANT_BUFFER);
        deviceResources->GetD3DDevice()->CreateBuffer(&bufferDescModel, nullptr, m_buffer.ReleaseAndGetAddressOf());
    }

    void Update(DeviceResources* deviceResources)
    {
        deviceResources->GetD3DDeviceContext()->UpdateSubresource(m_buffer.Get(), 0, nullptr, &m_data, 0, 0);
    }

    void ApplyToVS(DeviceResources* deviceResources, int slot = 0)
    {
        ID3D11Buffer* cbs[] = { m_buffer.Get() };
        deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
    }
};