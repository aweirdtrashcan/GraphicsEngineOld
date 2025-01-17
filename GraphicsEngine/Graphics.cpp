#include "Graphics.h"

#include "StimplyException.h"
#include "Macros.h"
#include "GraphicsFabric.h"
#include "Window.h"
#include "Texture.h"
#include "GraphicsDebug.h"
#include "HrException.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "GraphicsException.h"
#include "ConstantBuffer.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

const char* FeatureLevelToString(D3D_FEATURE_LEVEL level) {

	switch (level) {
		case D3D_FEATURE_LEVEL_1_0_GENERIC:
			return "1.0 generic";
		case D3D_FEATURE_LEVEL_1_0_CORE:
			return "1.0 core";
		case D3D_FEATURE_LEVEL_9_1:
			return "9.1";
		case D3D_FEATURE_LEVEL_9_2:
			return "9.2";
		case D3D_FEATURE_LEVEL_9_3:
			return "9.3";
		case D3D_FEATURE_LEVEL_10_0:
			return "10.0";
		case D3D_FEATURE_LEVEL_10_1:
			return "10.1";
		case D3D_FEATURE_LEVEL_11_0:
			return "11.0";
		case D3D_FEATURE_LEVEL_11_1:
			return "11.1";
		case D3D_FEATURE_LEVEL_12_0:
			return "12.0";
		case D3D_FEATURE_LEVEL_12_1:
			return "12.1";
		case D3D_FEATURE_LEVEL_12_2:
			return "12.2";
	}

	return "Unknown";
}

Graphics::Graphics(UINT width, UINT height)
	:
	m_Width(width),
	m_Height(height) {

	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}

	HR_THROW_FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_Factory)));

	ComPtr<IDXGIAdapter1> tempAdapter = nullptr;
	ComPtr<IDXGIAdapter1> selectedAdapter = nullptr;

#define HAS_DEVICE(i) m_Factory->EnumAdapters1(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND

	for (UINT i = 0; HAS_DEVICE(i); i++) {
		HRESULT s = D3D12CreateDevice(tempAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(s)) {
			selectedAdapter = tempAdapter;
		}
	}

	if (selectedAdapter == nullptr) {
		GRAPHICS_EXCEPTION(L"Failed to find a GPU capable of D3D12");
	}

	ComPtr<ID3D12Device> tempDevice = nullptr;

	HRESULT s = D3D12CreateDevice(selectedAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&tempDevice));
	if (FAILED(s)) {
		GRAPHICS_EXCEPTION(L"Failed to create a D3D12 device");
	}

	D3D_FEATURE_LEVEL requestedLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_2
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevel{};
	featureLevel.pFeatureLevelsRequested = requestedLevels;
	featureLevel.NumFeatureLevels = _countof(requestedLevels);

	HR_THROW_FAILED(tempDevice->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS,
		&featureLevel,
		sizeof(featureLevel)
	));

	s = D3D12CreateDevice(selectedAdapter.Get(), featureLevel.MaxSupportedFeatureLevel, IID_PPV_ARGS(&m_Device));
	if (FAILED(s)) {
		GRAPHICS_EXCEPTION(L"Failed to create a D3D12 device");
	} else {
		printf("Device created successfully with feature level %s!\n", FeatureLevelToString(featureLevel.MaxSupportedFeatureLevel));
	}

	s_GraphicsInstance = this;

	m_DirectCommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_DirectCommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, m_DirectCommandAllocator);
	m_DirectCommandQueue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_CopyCommandQueue = GraphicsFabric::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	m_GraphicsFence = GraphicsFabric::CreateFence();
	m_CopyFence = GraphicsFabric::CreateFence();

	s_SampleDesc = GraphicsFabric::QuerySampleDesc(s_BackBufferFormat);

	m_RenderTargetClearValue.Color[0] = 0.3f;
	m_RenderTargetClearValue.Color[1] = 0.3f;
	m_RenderTargetClearValue.Color[2] = 0.6f;
	m_RenderTargetClearValue.Color[3] = 1.0f;
	m_RenderTargetClearValue.Format = s_BackBufferFormat;

	m_Swapchain = CreateSwapchain(width, height);
	m_BackBuffers = GetSwapchainBuffers(m_Swapchain.Get(), m_RTVHeap, s_BufferCount, m_RTVIncrementSize);
	m_MSBackBuffers = CreateMultiSampleRenderTargets(width, height, m_RTVMSHeap);
	/*m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);*/

	m_Viewport.Height = static_cast<FLOAT>(height);
	m_Viewport.Width = static_cast<FLOAT>(width);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	m_Scissor.right = static_cast<LONG>(width);
	m_Scissor.bottom = static_cast<LONG>(height);

	DXGI_SAMPLE_DESC depthSample = GraphicsFabric::QuerySampleDesc(s_DepthStencilFormat);

	m_DepthBuffer = GraphicsFabric::CreateDepthBuffer(width, height, s_DepthStencilFormat, depthSample);
	CreateDepthDescriptor(m_DepthBuffer, m_DepthDescriptorHeap, depthSample);

	m_SrvDescHeap = GraphicsFabric::CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	ImGui_ImplDX12_Init(
		m_Device.Get(), s_BufferCount, s_BackBufferFormat,
		m_SrvDescHeap.Get(),
		m_SrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_SrvDescHeap->GetGPUDescriptorHandleForHeapStart()
	);
}

Graphics::Graphics(RECT windowRect) 
	:
	Graphics(windowRect.right, windowRect.bottom)
{}

Graphics::~Graphics() {
	ImGui_ImplDX12_Shutdown();
}

const Graphics::FramePrepData& Graphics::PrepareFrame() {
	HR_THROW_FAILED(m_DirectCommandAllocator->Reset());
	HR_THROW_FAILED(m_DirectCommandList->Reset(m_DirectCommandAllocator.Get(), nullptr));

	m_PrepData.frameNumber = m_Swapchain->GetCurrentBackBufferIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVMSHeap->GetCPUDescriptorHandleForHeapStart().At(m_PrepData.frameNumber, m_RTVIncrementSize);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_DepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	Barrier(m_MSBackBuffers[m_PrepData.frameNumber], D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Barrier(m_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	GFX_THROW_FAILED(m_DirectCommandList->ClearRenderTargetView(rtvHandle, m_RenderTargetClearValue.Color, 0u, NULL));
	GFX_THROW_FAILED(m_DirectCommandList->ClearDepthStencilView(
		m_DepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr
	));

	SetRenderTarget(m_PrepData.frameNumber, m_RTVMSHeap, m_DepthDescriptorHeap);

	GFX_THROW_FAILED(m_DirectCommandList->RSSetViewports(1u, &m_Viewport));
	GFX_THROW_FAILED(m_DirectCommandList->RSSetScissorRects(1u, &m_Scissor));

	PrepareImGuiFrame();

	return m_PrepData;
}

void Graphics::ExecuteCommandLists() {
	Barrier(m_MSBackBuffers[m_PrepData.frameNumber], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
	Barrier(m_BackBuffers[m_PrepData.frameNumber], D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RESOLVE_DEST);

	GFX_THROW_FAILED(m_DirectCommandList->ResolveSubresource(
		m_BackBuffers[m_PrepData.frameNumber].Get(),
		0,
		m_MSBackBuffers[m_PrepData.frameNumber].Get(),
		0,
		s_BackBufferFormat
	));

	Barrier(m_MSBackBuffers[m_PrepData.frameNumber], D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_COMMON);
	Barrier(m_BackBuffers[m_PrepData.frameNumber], D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Barrier(m_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);

	SetRenderTarget(m_PrepData.frameNumber, m_RTVHeap, m_DepthDescriptorHeap);

	// Render ImGui UI before the barrier RTVState -> Present State
	RenderImGuiFrame();

	Barrier(m_BackBuffers[m_PrepData.frameNumber], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	HR_THROW_FAILED(m_DirectCommandList->Close());

	ID3D12CommandList* cmdLists[] = { m_DirectCommandList.Get() };

	GFX_THROW_FAILED(m_DirectCommandQueue->ExecuteCommandLists(1u, cmdLists));
	GraphicsFabric::SignalFence(m_DirectCommandQueue, m_GraphicsFence, m_GraphicsFence);
	GraphicsFabric::WaitForFence(m_GraphicsFence, m_GraphicsFence);
}

void Graphics::Present() {
	m_Swapchain->Present(0u, DXGI_PRESENT_ALLOW_TEARING);
}

void Graphics::WaitDeviceIdle() {
	GraphicsFabric::WaitForFence(m_GraphicsFence, m_GraphicsFence);
}

ComPtr<IDXGISwapChain3> Graphics::CreateSwapchain(UINT width, UINT height) {
	ComPtr<IDXGISwapChain> swap;
	ComPtr<IDXGISwapChain3> swap3;
	
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
	desc.OutputWindow = Window::Get()->GetHandleToWindow();
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	HR_THROW_FAILED(m_Factory->CreateSwapChain(m_DirectCommandQueue.Get(), &desc, &swap));

	HR_THROW_FAILED(swap.As(&swap3));

	return swap3;
}

std::vector<ComPtr<ID3D12Resource>> Graphics::GetSwapchainBuffers(IDXGISwapChain3* swapchain, 
																  ComPtr<ID3D12DescriptorHeap>& descriptorHeap, UINT numBuffers,
																  UINT& descriptorIncrement) const {
	std::vector<ComPtr<ID3D12Resource>> buffers(numBuffers);

	if (m_RTVHeap == nullptr) {
		descriptorHeap = GraphicsFabric::CreateDescriptorHeap(s_BufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		descriptorIncrement = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	for (UINT i = 0; i < numBuffers; i++) {
		ComPtr<ID3D12Resource> currBuffer;
		HR_THROW_FAILED(swapchain->GetBuffer(i, IID_PPV_ARGS(&currBuffer)));
		buffers[i] = std::move(currBuffer);

		GFX_THROW_FAILED(m_Device->CreateRenderTargetView(
			buffers[i].Get(),
			nullptr,
			descriptorHeap->GetCPUDescriptorHandleForHeapStart().At(i, descriptorIncrement)
		));
	}

	return buffers;
}

std::vector<ComPtr<ID3D12Resource>> Graphics::CreateMultiSampleRenderTargets(UINT width, UINT height, ComPtr<ID3D12DescriptorHeap>& descriptorHeap) const {
	std::vector<ComPtr<ID3D12Resource>> buffers(m_BackBuffers.size());

	if (descriptorHeap == nullptr) {
		descriptorHeap = GraphicsFabric::CreateDescriptorHeap(static_cast<UINT>(m_BackBuffers.size()), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	for (UINT i = 0; i < m_BackBuffers.size(); i++) {
		buffers[i] = GraphicsFabric::CreateRenderTargetTexture(
			width, height,
			s_BackBufferFormat, s_SampleDesc,
			m_RenderTargetClearValue
		);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = s_BackBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

		GFX_THROW_FAILED(m_Device->CreateRenderTargetView(
			buffers[i].Get(),
			&rtvDesc,
			descriptorHeap->GetCPUDescriptorHandleForHeapStart().At(i, m_RTVIncrementSize)
		));
	}

	return buffers;
}

void Graphics::CreateDepthDescriptor(ComPtr<ID3D12Resource> depthBuffer, ComPtr<ID3D12DescriptorHeap>& outDescriptorHeap, DXGI_SAMPLE_DESC sample) {
	if (outDescriptorHeap == nullptr) {
		outDescriptorHeap = GraphicsFabric::CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	D3D12_DSV_DIMENSION dimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	if (sample.Count > 1) {
		dimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.ViewDimension = dimension;
	desc.Texture2D.MipSlice = 0;
	desc.Flags = D3D12_DSV_FLAG_NONE;
	desc.Format = s_DepthStencilFormat;

	GFX_THROW_FAILED(m_Device->CreateDepthStencilView(depthBuffer.Get(), &desc, outDescriptorHeap->GetCPUDescriptorHandleForHeapStart()));
}

void Graphics::Barrier(ComPtr<ID3D12Resource>& res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	GFX_THROW_FAILED(m_DirectCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		res.Get(),
		before,
		after
	)));
}

void Graphics::SetRenderTarget(UINT numFrame, ComPtr<ID3D12DescriptorHeap> rtv, ComPtr<ID3D12DescriptorHeap> dsv) {
	GFX_THROW_FAILED(m_DirectCommandList->OMSetRenderTargets(
		1, &rtv->GetCPUDescriptorHandleForHeapStart().At(numFrame, m_RTVIncrementSize),
		FALSE, 
		nullptr
	));
}

void Graphics::PrepareImGuiFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Graphics::RenderImGuiFrame() {	
	if (ImGui::Begin("Status")) {
		ImGuiIO& io = ImGui::GetIO();

		UINT frameRate = (UINT)io.Framerate;
		m_PrepData.deltaTime = io.DeltaTime;

		ImGui::Text("FPS: %d", frameRate);
		ImGui::Text("DeltaTime: %f", io.DeltaTime);
		ImGui::Text("MSAA %dX", s_SampleDesc.Count);
	}

	ImGui::End();

	ImGui::Render();
	
	GFX_THROW_FAILED(m_DirectCommandList->SetDescriptorHeaps(1, m_SrvDescHeap.GetAddressOf()));

	ImDrawData* drawData = ImGui::GetDrawData();
	ImGui_ImplDX12_RenderDrawData(drawData, m_DirectCommandList.Get());
}
