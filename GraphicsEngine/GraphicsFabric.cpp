#include "GraphicsFabric.h"

#include "Texture.h"
#include "GraphicsException.h"

ComPtr<ID3D12Resource> GraphicsFabric::CreateTextureImage(void* textureImage) {
    Texture::TextureImage& image = *reinterpret_cast<Texture::TextureImage*>(textureImage);

    ComPtr<ID3D12Resource> textureBuffer;

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
    ComPtr<ID3D12GraphicsCommandList> copyCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, copyCommandAllocator);
    HR_THROW_FAILED(copyCommandAllocator->Reset());
    HR_THROW_FAILED(copyCommandList->Reset(copyCommandAllocator.Get(), nullptr));

    HR_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(image.GetTextureSize()),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&textureBuffer)
    ));

    GFX_THROW_FAILED(copyCommandList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            textureBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST
        )
    ));

    ComPtr<ID3D12Resource> uploadBuffer = GraphicsFabric::CreateUploadBuffer(
        copyCommandList,
        image.GetTextureSize()
    );

    void* uploadData;
    HR_THROW_FAILED(uploadBuffer->Map(0, nullptr, &uploadData));

    memcpy(uploadData, image.GetImageRawData(), image.GetTextureSize());
    
    GFX_THROW_FAILED(copyCommandList->CopyResource(textureBuffer.Get(), uploadBuffer.Get()));

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

    HR_THROW_FAILED(copyCommandList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)copyCommandList.GetAddressOf());
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    GFX_THROW_FAILED(uploadBuffer->Unmap(0, nullptr));

    return textureBuffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateUploadBuffer(ComPtr<ID3D12GraphicsCommandList> resettedCommandList, 
                                                          SIZE_T bufferSize) {
    ComPtr<ID3D12Resource> uploadBuffer;

    HR_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
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

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
    ComPtr<ID3D12GraphicsCommandList> copyCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, copyCommandAllocator);
    HR_THROW_FAILED(copyCommandAllocator->Reset());
    HR_THROW_FAILED(copyCommandList->Reset(copyCommandAllocator.Get(), nullptr));

    ComPtr<ID3D12Resource> uploadBuffer = GraphicsFabric::CreateUploadBuffer(copyCommandList, bufferSize);

    HR_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&buffer)
    ));

    void* mappedUpload = nullptr;
    HR_THROW_FAILED(uploadBuffer->Map(0, nullptr, &mappedUpload));
    memcpy(mappedUpload, data, bufferSize);
    GFX_THROW_FAILED(uploadBuffer->Unmap(0, nullptr));

    GFX_THROW_FAILED(copyCommandList->CopyResource(buffer.Get(), uploadBuffer.Get()));

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

    if (bIndex) {
        GFX_THROW_FAILED(copyCommandList->ResourceBarrier(
            1,
            &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER)
        ));
    }

    HR_THROW_FAILED(copyCommandList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    GFX_THROW_FAILED(queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)copyCommandList.GetAddressOf()));
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateGenericBuffer(SIZE_T bufferSize) {
    ComPtr<ID3D12Resource> buffer;

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
    ComPtr<ID3D12GraphicsCommandList> copyCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, copyCommandAllocator);
    HR_THROW_FAILED(copyCommandAllocator->Reset());
    HR_THROW_FAILED(copyCommandList->Reset(copyCommandAllocator.Get(), nullptr));

    HR_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&buffer)
    ));

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

    HR_THROW_FAILED(copyCommandList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    GFX_THROW_FAILED(queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)copyCommandList.GetAddressOf()));
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateDepthBuffer(UINT width, UINT height, DXGI_FORMAT depthFormat, DXGI_SAMPLE_DESC sampleDesc) {
    ComPtr<ID3D12Resource> buffer;

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
    ComPtr<ID3D12GraphicsCommandList> cmdList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, copyCommandAllocator);
    HR_THROW_FAILED(copyCommandAllocator->Reset());
    HR_THROW_FAILED(cmdList->Reset(copyCommandAllocator.Get(), nullptr));

    D3D12_CLEAR_VALUE clearVal = {};
    clearVal.DepthStencil.Depth = 1.0f;
    clearVal.DepthStencil.Stencil = 0;
    clearVal.Format = depthFormat;

    HR_THROW_FAILED(gfx().m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(depthFormat, width, height, 1, 1, sampleDesc.Count, sampleDesc.Quality - 1, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_COMMON,
        &clearVal,
        IID_PPV_ARGS(&buffer)
    ));

    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

    GFX_THROW_FAILED(cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        buffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_READ
    )));

    HR_THROW_FAILED(cmdList->Close());

    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    GFX_THROW_FAILED(queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)cmdList.GetAddressOf()));
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    return buffer;
}

ComPtr<ID3D12Resource> GraphicsFabric::CreateRenderTargetTexture(UINT width, UINT height, DXGI_FORMAT format, 
                                                     DXGI_SAMPLE_DESC sampleInfo, const D3D12_CLEAR_VALUE& clearVal,
                                                     D3D12_RESOURCE_STATES state) {
    ComPtr<ID3D12Resource> textureBuffer;

    HR_THROW_FAILED(Graphics::GetGraphics()->m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(
            format, 
            (UINT64)width, 
            (UINT64)height, 
            1,
            1, 
            sampleInfo.Count, 
            sampleInfo.Quality - 1, 
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
        D3D12_RESOURCE_STATE_COMMON,
        &clearVal,
        IID_PPV_ARGS(&textureBuffer)
    ));

    if (state != D3D12_RESOURCE_STATE_COMMON) {
        ComPtr<ID3D12CommandAllocator> cmdAlloc = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
        ComPtr<ID3D12GraphicsCommandList> cmdList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc);
        HR_THROW_FAILED(cmdAlloc->Reset());
        HR_THROW_FAILED(cmdList->Reset(cmdAlloc.Get(), nullptr));

        GFX_THROW_FAILED(cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            textureBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, state
        )));

        HR_THROW_FAILED(cmdList->Close());

        ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
        UINT64 v = 0;

        ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

        GFX_THROW_FAILED(queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)cmdList.GetAddressOf()));
        SignalFence(queue, fence, v);
        WaitForFence(fence, v);
    }

    return textureBuffer;
}
