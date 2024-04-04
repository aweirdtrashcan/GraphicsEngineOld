#include "Graphics.h"

#include "StimplyException.h"
#include "Macros.h"
#include "GraphicsFabric.h"
#include "Window.h"
#include "Texture.h"
#include "GraphicsDebug.h"
#include "HrException.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

Graphics::Graphics(UINT width, UINT height) 
	:
	m_Width(width),
	m_Height(height) {
	
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}

	if (!SUCCEEDED(D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_Device)
	))) {
		GRAPHICS_EXCEPTION(L"Failed to create d3d12 device");
	}

	GFX_THROW_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_Factory)));

	s_GraphicsInstance = this;
	
	m_DirectCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_DirectCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, m_DirectCommandAllocator);
	m_DirectCommandQueue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_GraphicsFence = GraphicsFabric::CreateFence();
	m_Swapchain = CreateSwapchain(width, height);
	m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_Viewport.Height = static_cast<FLOAT>(height);
	m_Viewport.Width = static_cast<FLOAT>(width);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_Scissor.right = static_cast<LONG>(width);
	m_Scissor.bottom = static_cast<LONG>(height);
}

Graphics::Graphics(RECT windowRect) 
	:
	Graphics(windowRect.right, windowRect.bottom)
{}

Graphics::~Graphics() {}

void Graphics::ExecuteCommandLists(ID3D12CommandList** commandLists, UINT numCommandLists) {

	for (UINT i = 0; i < numCommandLists; i++) {
		ID3D12GraphicsCommandList* cmdList = static_cast<ID3D12GraphicsCommandList*>(commandLists[i]);
#if IS_DEBUG
	if (cmdList == nullptr) {
		__debugbreak();
	}
#endif
	GFX_THROW_FAILED(cmdList->Close());
	m_DirectCommandList->ExecuteBundle(cmdList);
	}

	m_DirectCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_BackBuffers[m_BackBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

	GFX_THROW_FAILED(m_DirectCommandList->Close());

	ID3D12CommandList* cmdList = m_DirectCommandList.Get();

	m_DirectCommandQueue->ExecuteCommandLists(1u, &cmdList);
	GraphicsFabric::SignalFence(m_DirectCommandQueue, m_GraphicsFence, m_FenceValue);
	GraphicsFabric::WaitForFence(m_GraphicsFence, m_FenceValue);
	m_BackBufferIndex = ++m_BackBufferIndex % s_BufferCount;
}

void Graphics::Present() {
	m_Swapchain->Present(0u, DXGI_PRESENT_ALLOW_TEARING);
}

void Graphics::PrepareFrame() {
	GFX_THROW_FAILED(m_DirectCommandAllocator->Reset());
	GFX_THROW_FAILED(m_DirectCommandList->Reset(m_DirectCommandAllocator.Get(), nullptr));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart().At(m_BackBufferIndex, m_RTVIncrementSize);

	m_DirectCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_BackBuffers[m_BackBufferIndex].Get(), 
			D3D12_RESOURCE_STATE_COMMON, 
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

	FLOAT rtvColor[] = {0.3f, 0.3f, 0.6f, 1.0f};
	m_DirectCommandList->ClearRenderTargetView(rtvHandle, rtvColor, 0u, NULL);
	m_DirectCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);

	m_DirectCommandList->RSSetViewports(1u, &m_Viewport);
	m_DirectCommandList->RSSetScissorRects(1u, &m_Scissor);
}

void Graphics::WaitDeviceIdle() {
	GraphicsFabric::WaitForFence(m_GraphicsFence, m_FenceValue);
}

ComPtr<IDXGISwapChain2> Graphics::CreateSwapchain(UINT width, UINT height) {
	ComPtr<IDXGISwapChain> swap;
	ComPtr<IDXGISwapChain2> swap2;
	
	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferCount = s_BufferCount;
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = s_BackBufferFormat;
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferDesc.RefreshRate.Numerator = 1;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.Windowed = TRUE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = Window::Get().GetHandleToWindow();
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	GFX_THROW_FAILED(m_Factory->CreateSwapChain(
		m_DirectCommandQueue.Get(),
		&desc,
		&swap
	));

	GFX_THROW_FAILED(swap.As(&swap2));

	if (m_RTVHeap == nullptr) {
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NumDescriptors = s_BufferCount;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		GFX_THROW_FAILED(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RTVHeap)));

		m_RTVIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	for (UINT i = 0; i < s_BufferCount; i++) {
		GFX_THROW_FAILED(swap2->GetBuffer(i, IID_PPV_ARGS(&m_BackBuffers[i])));

		m_Device->CreateRenderTargetView(
			m_BackBuffers[i].Get(),
			nullptr,
			m_RTVHeap->GetCPUDescriptorHandleForHeapStart().At(i, m_RTVIncrementSize)
		);
	}

	return swap2;
}

std::vector<ComPtr<ID3D12Resource>> Graphics::GetSwapchainBuffers(ComPtr<IDXGISwapChain2> swapchain, UINT numBuffers) const {
	std::vector<ComPtr<ID3D12Resource>> buffers(numBuffers);

	for (UINT i = 0; i < numBuffers; i++) {
		ComPtr<ID3D12Resource> currBuffer;
		GFX_THROW_FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&currBuffer)));
		buffers[i] = std::move(currBuffer);
	}

	return buffers;
}

Graphics::GraphicsException::GraphicsException(int line, const wchar_t* file, std::wstring_view reason) 
	:
	StimplyException(line, file, reason)
{}

const wchar_t* Graphics::GraphicsException::GetType() const noexcept {
	return L"Graphics Exception";
}
