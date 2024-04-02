#pragma once

#include "Graphics.h"
#include "Macros.h"
#include "D3DIncls.inl"

#include <unordered_map>

class GraphicsFabric {
public:
	static GraphicsFabric& Get() {
		static GraphicsFabric instance;
		return instance;
	}

	static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type) {
		Graphics* gfx = Graphics::GetGraphics();
		if (!gfx) {
			return nullptr;
		}

		ComPtr<ID3D12CommandAllocator> allocator;

		GFX_THROW_FAILED(gfx->m_Device->CreateCommandAllocator(
			type,
			IID_PPV_ARGS(&allocator)
		));

		return allocator;
	}

	static ComPtr<ID3D12GraphicsCommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12CommandAllocator> allocator) {
		Graphics* gfx = Graphics::GetGraphics();
	
		ComPtr<ID3D12GraphicsCommandList> cmdList;

		GFX_THROW_FAILED(
			gfx->m_Device->CreateCommandList(
			0, type, allocator.Get(),
			nullptr,
			IID_PPV_ARGS(&cmdList))
		);

		GFX_THROW_FAILED(cmdList->Close());

		return cmdList;
	}

	static ComPtr<ID3D12CommandQueue> CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
		Graphics* gfx = Graphics::GetGraphics();
		GraphicsFabric& fabric = GraphicsFabric::Get();

		if (fabric.m_CommandQueue[type] == nullptr) {
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = type;

			GFX_THROW_FAILED(gfx->m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&fabric.m_CommandQueue[type])));
		}

		return fabric.m_CommandQueue[type];
	}

	static ComPtr<ID3D12Fence> CreateFence(UINT64 initialValue = 0) {
		Graphics* gfx = Graphics::GetGraphics();

		ComPtr<ID3D12Fence> fence;

		GFX_THROW_FAILED(gfx->m_Device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		return fence;
	}

	static void WaitForFence(ComPtr<ID3D12Fence> fence, UINT64 value) {
		HANDLE event = CreateEventA(NULL, FALSE, FALSE, NULL);
		if (fence->GetCompletedValue() < value) {
			GFX_THROW_FAILED(fence->SetEventOnCompletion(value, event));
			WaitForSingleObject(event, DWORD_MAX);
		}
		CloseHandle(event);
	}

	static void SignalFence(ComPtr<ID3D12CommandQueue> queue, ComPtr<ID3D12Fence> fence, UINT64& value) {
		GFX_THROW_FAILED(queue->Signal(fence.Get(), ++value));
	}

	/// <summary>
	/// Thread-blocking function to create a Texture.
	/// </summary>
	static ComPtr<ID3D12Resource> CreateTexture(void* textureImage);

private:
	std::unordered_map<D3D12_COMMAND_LIST_TYPE, ComPtr<ID3D12CommandQueue>> m_CommandQueue;
};