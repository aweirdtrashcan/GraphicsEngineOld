#include "GraphicsFabric.h"

#include "Texture.h"

ComPtr<ID3D12Resource> GraphicsFabric::CreateTexture(void* textureImage) {
    Graphics& gfx = *Graphics::GetGraphics();
    ComPtr<ID3D12Device1>& device = gfx.m_Device;
    Texture::TextureImage& image = *reinterpret_cast<Texture::TextureImage*>(textureImage);

    ComPtr<ID3D12Resource> textureBuffer;

    ComPtr<ID3D12CommandAllocator> copyCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY);
    ComPtr<ID3D12GraphicsCommandList> copyCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, copyCommandAllocator);
    GFX_THROW_FAILED(copyCommandList->Reset(copyCommandAllocator.Get(), nullptr));

    GFX_THROW_FAILED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(image.GetTextureSize()),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&textureBuffer)
    ));

    ComPtr<ID3D12Resource> uploadBuffer;

    GFX_THROW_FAILED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(image.GetTextureSize()),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    ));


    copyCommandList->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            textureBuffer.Get(), 
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_COPY_DEST
        )
    );


    void* uploadData;
    GFX_THROW_FAILED(uploadBuffer->Map(0, nullptr, &uploadData));

    memcpy(uploadData, image.GetImageRawData(), image.GetTextureSize());
    
    ComPtr<ID3D12CommandQueue> queue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);


    ComPtr<ID3D12Fence> fence = GraphicsFabric::CreateFence();
    UINT64 v = 0;
    queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)copyCommandList.GetAddressOf());
    SignalFence(queue, fence, v);
    WaitForFence(fence, v);

    return textureBuffer;
}
