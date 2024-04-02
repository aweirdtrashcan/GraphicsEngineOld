#include "GraphicsFabric.h"

#include "Texture.h"

ComPtr<ID3D12Resource> GraphicsFabric::CreateTexture(void* textureImage) {
    Graphics& gfx = *Graphics::GetGraphics();
    Texture::TextureImage& image = *reinterpret_cast<Texture::TextureImage*>(textureImage);

    ComPtr<ID3D12Resource> textureBuffer;

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY);
    ComPtr<ID3D12GraphicsCommandList> copyCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, copyCommandAllocator);
    GFX_THROW_FAILED(copyCommandList->Reset(copyCommandAllocator.Get(), nullptr));

    GFX_THROW_FAILED(gfx.m_Device->CreateCommittedResource(
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
    Graphics& gfx = *Graphics::GetGraphics();

    GFX_THROW_FAILED(gfx.m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    ));

    return uploadBuffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateVertexBuffer(ComPtr<ID3D12GraphicsCommandList> resettedCommandList, const void* vertices, 
                                                          SIZE_T verticesCount, SIZE_T stride, D3D12_VERTEX_BUFFER_VIEW& outView) {
    ComPtr<ID3D12Resource> buffer = GraphicsFabric::CreateGenericBuffer(resettedCommandList, vertices, verticesCount, stride);

    outView.BufferLocation = buffer->GetGPUVirtualAddress();
    outView.SizeInBytes = verticesCount * stride;
    outView.StrideInBytes = stride;

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateIndexBuffer(ComPtr<ID3D12GraphicsCommandList> resettedCommandList, const void* indices, 
                                                         SIZE_T indexCount, SIZE_T stride, D3D12_INDEX_BUFFER_VIEW& outView) {
    if (stride > 8 || stride < 4) {
        STIMPLY_EXCEPTION(L"Can't have index buffer whose stride is bigger than 8 bytes (32 bits)!");
    }
    ComPtr<ID3D12Resource> buffer = GraphicsFabric::CreateGenericBuffer(resettedCommandList, indices, indexCount, stride);

    outView.BufferLocation = buffer->GetGPUVirtualAddress();
    outView.SizeInBytes = indexCount * stride;
    outView.Format = stride == 8 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateGenericBuffer(ComPtr<ID3D12GraphicsCommandList> resettedCommandList, const void* data, SIZE_T bufferSize, SIZE_T stride) {
    Graphics& gfx = *Graphics::GetGraphics();
    ComPtr<ID3D12Resource> uploadBuffer = GraphicsFabric::CreateUploadBuffer(resettedCommandList, bufferSize);

    ComPtr<ID3D12Resource> buffer;

    GFX_THROW_FAILED(gfx.m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize * stride),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&buffer)
    ));

    void* mappedUpload = nullptr;
    GFX_THROW_FAILED(uploadBuffer->Map(0, nullptr, &mappedUpload));
    memcpy(mappedUpload, data, bufferSize);
    uploadBuffer->Unmap(0, nullptr);

    resettedCommandList->CopyResource(buffer.Get(), uploadBuffer.Get());

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

    GFX_THROW_FAILED(resettedCommandList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)resettedCommandList.GetAddressOf());
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    return buffer;
}
