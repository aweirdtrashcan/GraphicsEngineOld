#include "Graphics.h"

#include "StimplyException.h"
#include "Macros.h"
#include "GraphicsFabric.h"
#include "Window.h"
#include "Texture.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

Graphics* Graphics::s_GraphicsInstance = nullptr;
UINT Graphics::s_BufferCount = 2;
DXGI_FORMAT Graphics::s_BackBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
DXGI_FORMAT Graphics::s_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

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
	m_Swapchain = CreateSwapchain(width, height);
	m_GraphicsFence = GraphicsFabric::CreateFence();
	auto r = Texture(m_DirectCommandList, m_DirectCommandAllocator, "M_Frame_D.bmp");
}

Graphics::Graphics(RECT windowRect) 
	:
	Graphics(windowRect.right, windowRect.bottom)
{
	
}

ComPtr<IDXGISwapChain2> Graphics::CreateSwapchain(UINT width, UINT height) const {
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
