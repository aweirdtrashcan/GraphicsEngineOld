#pragma once

#include "D3DIncls.inl"
#include "StimplyException.h"

class Graphics {
	friend class GraphicsFabric;
public:
	class GraphicsException : StimplyException {
	public:
		GraphicsException(int line, const wchar_t* file, std::wstring_view reason);
		virtual const wchar_t* GetType() const noexcept override;
	};
public:
	Graphics(UINT width, UINT height);
	Graphics(RECT windowRect);
	Graphics(const Graphics&) = delete;
	~Graphics() = default;

private:
	// Function for GaphicsFabric
	static Graphics* GetGraphics() { return s_GraphicsInstance; }

	ComPtr<IDXGISwapChain2> CreateSwapchain(UINT width, UINT height) const;
	std::vector<ComPtr<ID3D12Resource>> GetSwapchainBuffers(ComPtr<IDXGISwapChain2> swapchain, UINT numBuffers) const;

private:
	static UINT s_BufferCount;
	static DXGI_FORMAT s_BackBufferFormat;
	static DXGI_FORMAT s_DepthStencilFormat;
	static Graphics* s_GraphicsInstance;

	UINT m_Width;
	UINT m_Height;
	ComPtr<IDXGIFactory1> m_Factory;
	ComPtr<ID3D12Device1> m_Device;
	ComPtr<IDXGISwapChain2> m_Swapchain;
	ComPtr<ID3D12CommandAllocator> m_DirectCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_DirectCommandList;
	ComPtr<ID3D12CommandQueue> m_DirectCommandQueue;
	std::vector<ComPtr<ID3D12Resource>> m_BackBuffers;
	ComPtr<ID3D12Fence> m_GraphicsFence;
};

