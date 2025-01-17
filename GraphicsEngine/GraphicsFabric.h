#pragma once

#include "Graphics.h"
#include "Macros.h"
#include "D3DIncls.inl"
#include "GraphicsDebug.h"
#include "HrException.h"

#include <unordered_map>
#include <queue>
#include <sstream>

#include "GraphicsException.h"

class GraphicsFabric {
public:
	static GraphicsFabric& Get() {
		static GraphicsFabric instance;
		return instance;
	}

	static ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type) {
		ComPtr<ID3D12CommandAllocator> allocator;

		HR_THROW_FAILED(gfx().m_Device->CreateCommandAllocator(
			type,
			IID_PPV_ARGS(&allocator)
		));

		return allocator;
	}

	static ComPtr<ID3D12GraphicsCommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12CommandAllocator> allocator) {
		ComPtr<ID3D12GraphicsCommandList> cmdList;

		HR_THROW_FAILED(
			gfx().m_Device->CreateCommandList(
			0, type, allocator.Get(),
			nullptr,
			IID_PPV_ARGS(&cmdList))
		);

		HR_THROW_FAILED(cmdList->Close());

		return cmdList;
	}

	static ComPtr<ID3D12CommandQueue> CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type) {
		GraphicsFabric& fabric = GraphicsFabric::Get();

		if (fabric.m_CommandQueue[type] == nullptr) {
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = type;

			HR_THROW_FAILED(gfx().m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&fabric.m_CommandQueue[type])));
		}

		return fabric.m_CommandQueue[type];
	}

	static ComPtr<ID3D12Fence> CreateFence(UINT64 initialValue = 0) {
		ComPtr<ID3D12Fence> fence;

		HR_THROW_FAILED(gfx().m_Device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		return fence;
	}

	static void WaitForFence(ComPtr<ID3D12Fence> fence, UINT64 value) {
		HANDLE event = CreateEventA(NULL, FALSE, FALSE, NULL);
		if (fence->GetCompletedValue() < value) {
			HR_THROW_FAILED(fence->SetEventOnCompletion(value, event));
			WaitForSingleObject(event, DWORD_MAX);
		}
		CloseHandle(event);
	}

	static void WaitForFence(ComPtr<ID3D12Fence> fence, UINT64 value, HANDLE event) {
		if (fence->GetCompletedValue() < value) {
			HR_THROW_FAILED(fence->SetEventOnCompletion(value, event));
			WaitForSingleObject(event, DWORD_MAX);
		}
	}

	static void SignalFence(ComPtr<ID3D12CommandQueue> queue, ComPtr<ID3D12Fence> fence, UINT64& value) {
		HR_THROW_FAILED(queue->Signal(fence.Get(), ++value));
	}

	/// <summary>
	/// Thread-blocking function to create a Texture.
	/// </summary>
	static ComPtr<ID3D12Resource> CreateTextureImage(void* textureImage);
	static ComPtr<ID3D12Resource> CreateUploadBuffer(ComPtr<ID3D12GraphicsCommandList> resettedCommandList, SIZE_T bufferSize);
	/// <summary>
	/// Returns a vertex buffer.
	/// </summary>
	static ComPtr<ID3D12Resource> CreateVertexBuffer(const void* vertices,
													 SIZE_T verticesCount, SIZE_T stride, D3D12_VERTEX_BUFFER_VIEW& outView);

	static ComPtr<ID3D12Resource> CreateIndexBuffer(const void* indices,
													SIZE_T indexCount, SIZE_T stride, D3D12_INDEX_BUFFER_VIEW& outView);

	static ComPtr<ID3D12Resource> CreateGenericBuffer(const void* data,
													  SIZE_T bufferSize, SIZE_T stride, bool bIndex = false);
	static ComPtr<ID3D12Resource> CreateGenericBuffer(SIZE_T bufferSize);
	static ComPtr<ID3D12Resource> CreateDepthBuffer(UINT width, UINT height, DXGI_FORMAT depthFormat, DXGI_SAMPLE_DESC sampleDesc);
	
	static ComPtr<ID3D12PipelineState> CreatePipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
		ComPtr<ID3D12PipelineState> state;

		HR_THROW_FAILED(gfx().m_Device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&state)));
		return state;
	}

	static ComPtr<ID3D12RootSignature> CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc) {
		ComPtr<ID3DBlob> errorBlob;
		ComPtr<ID3DBlob> successBlob;

		if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &successBlob, &errorBlob))) {
			const char* errorMessage = static_cast<const char*>(errorBlob->GetBufferPointer());

			std::wstringstream oss;

			oss << errorMessage;

			GRAPHICS_EXCEPTION(oss.str());
		}

		ComPtr<ID3D12RootSignature> rootSignature;

		HR_THROW_FAILED(
			gfx().m_Device->CreateRootSignature(
				0, 
				successBlob->GetBufferPointer(), 
				successBlob->GetBufferSize(), 
				IID_PPV_ARGS(&rootSignature)
		));

		return rootSignature;
	}

	static ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type) {
		ComPtr<ID3D12DescriptorHeap> dHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = type;
		desc.NumDescriptors = descriptorCount;
		desc.Flags = (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) ? 
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		
		HR_THROW_FAILED(Graphics::GetGraphics()->m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dHeap)));

		return dHeap;
	}

	static ComPtr<ID3D12Resource> CreateRenderTargetTexture(UINT width, UINT height, DXGI_FORMAT format, 
															DXGI_SAMPLE_DESC sampleDesc, const D3D12_CLEAR_VALUE& clearVal);

	static DXGI_SAMPLE_DESC QuerySampleDesc(DXGI_FORMAT format) {
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQuality;
		msQuality.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQuality.Format = format;
		msQuality.SampleCount = 2;
		msQuality.NumQualityLevels = 0;

		HR_THROW_FAILED(gfx().m_Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQuality,
			sizeof(msQuality)
		));

		DXGI_SAMPLE_DESC sd;
		sd.Count = msQuality.SampleCount;
		sd.Quality = msQuality.NumQualityLevels;
		if (sd.Quality > 0) {
			sd.Quality--;
		}

		return sd;
	}

	static constexpr UINT64 CalculateConstantBufferSize(UINT64 size) {
		return (size + 255) & ~255;
	}

	static void CreateConstantBufferView(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT size,
										 D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = bufferLocation;
		desc.SizeInBytes = size;

		GFX_THROW_FAILED(gfx().m_Device->CreateConstantBufferView(&desc, cpuHandle));
	}

	static UINT GetDescriptorIncrement(D3D12_DESCRIPTOR_HEAP_TYPE type) {
		return gfx().m_Device->GetDescriptorHandleIncrementSize(type);
	}

private:
	__forceinline static Graphics& gfx() {
		return *Graphics::GetGraphics();
	}

	GraphicsFabric() {
		m_CommandAllocator = CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_CommandList = CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get());
		
		m_ConstantBufferHeap = CreateDescriptorHeap(1000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_ConstantBufferUsage = 0;
	}

private:
	std::unordered_map<D3D12_COMMAND_LIST_TYPE, ComPtr<ID3D12CommandQueue>> m_CommandQueue;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12DescriptorHeap> m_ConstantBufferHeap;
	UINT64 m_ConstantBufferUsage;
};