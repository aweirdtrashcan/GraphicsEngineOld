#include "GraphicsFabric.h"

#include "Texture.h"

ComPtr<ID3D12Resource> GraphicsFabric::CreateTexture(void* textureImage) {
    Texture::TextureImage& image = *reinterpret_cast<Texture::TextureImage*>(textureImage);

    ComPtr<ID3D12Resource> textureBuffer;

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY);
    ComPtr<ID3D12GraphicsCommandList> copyCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, copyCommandAllocator);
    GFX_THROW_FAILED(copyCommandList->Reset(copyCommandAllocator.Get(), nullptr));

    GFX_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(image.GetTextureSize()),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&textureBuffer)
    ));

    copyCommandList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            textureBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST
        )
    );

    ComPtr<ID3D12Resource> uploadBuffer = GraphicsFabric::CreateUploadBuffer(
        copyCommandList,
        image.GetTextureSize()
    );

    void* uploadData;
    GFX_THROW_FAILED(uploadBuffer->Map(0, nullptr, &uploadData));

    memcpy(uploadData, image.GetImageRawData(), image.GetTextureSize());
    
    copyCommandList->CopyResource(textureBuffer.Get(), uploadBuffer.Get());

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

    GFX_THROW_FAILED(copyCommandList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)copyCommandList.GetAddressOf());
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    uploadBuffer->Unmap(0, nullptr);

    return textureBuffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateUploadBuffer(ComPtr<ID3D12GraphicsCommandList> resettedCommandList, 
                                                          SIZE_T bufferSize) {
    ComPtr<ID3D12Resource> uploadBuffer;

    GFX_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    ));

    return uploadBuffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateVertexBuffer(const void* vertices, 
                                                          SIZE_T verticesCount, SIZE_T stride, D3D12_VERTEX_BUFFER_VIEW& outView) {
    ComPtr<ID3D12Resource> buffer = GraphicsFabric::CreateGenericBuffer(vertices, verticesCount, stride);

    outView.BufferLocation = buffer->GetGPUVirtualAddress();
    outView.SizeInBytes = static_cast<UINT>(verticesCount * stride);
    outView.StrideInBytes = static_cast<UINT>(stride);

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateIndexBuffer(const void* indices, 
                                                         SIZE_T indexCount, SIZE_T stride, D3D12_INDEX_BUFFER_VIEW& outView) {
    if (stride > 4 || stride < 2) {
        STIMPLY_EXCEPTION(L"Can't have index buffer whose stride is bigger than 4 bytes (32 bits)!");
    }
    ComPtr<ID3D12Resource> buffer = GraphicsFabric::CreateGenericBuffer(indices, indexCount, stride, true);

    outView.BufferLocation = buffer->GetGPUVirtualAddress();
    outView.SizeInBytes = static_cast<UINT>(indexCount * stride);
    outView.Format = stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateGenericBuffer(const void* data, SIZE_T bufferSize, SIZE_T stride, bool bIndex) {
    ComPtr<ID3D12Resource> buffer;

    bufferSize = bufferSize * stride;

    GraphicsFabric& f = GraphicsFabric::Get();
    GFX_THROW_FAILED(f.m_CommandAllocator->Reset());
    GFX_THROW_FAILED(f.m_CommandList->Reset(f.m_CommandAllocator.Get(), nullptr));

    ComPtr<ID3D12Resource> uploadBuffer = GraphicsFabric::CreateUploadBuffer(f.m_CommandList, bufferSize);

    GFX_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&buffer)
    ));

    void* mappedUpload = nullptr;
    GFX_THROW_FAILED(uploadBuffer->Map(0, nullptr, &mappedUpload));
    memcpy(mappedUpload, data, bufferSize);
    uploadBuffer->Unmap(0, nullptr);

    f.m_CommandList->CopyResource(buffer.Get(), uploadBuffer.Get());

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

    if (bIndex) {
        f.m_CommandList->ResourceBarrier(
            1,
            &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER)
        );
    }

    GFX_THROW_FAILED(f.m_CommandList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)f.m_CommandList.GetAddressOf());
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    return buffer;
}
