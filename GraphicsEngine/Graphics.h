#pragma once

#include "D3DIncls.inl"
#include "StimplyException.h"

class Graphics {
	friend class GraphicsFabric;
public:
	class Fence {
	public:
		Fence() : m_Fence(nullptr), m_Value(0) {}
		Fence(ComPtr<ID3D12Fence> fence) : m_Fence(fence.Get()), m_Value(0) {}
		ID3D12Fence* operator->() { return m_Fence.Get(); }
		operator ComPtr<ID3D12Fence>() { return m_Fence.Get(); }
		operator SIZE_T&() { return m_Value; }
	private:
		ComPtr<ID3D12Fence> m_Fence;
		SIZE_T m_Value;
	};
public:
	Graphics(UINT width, UINT height);
	Graphics(RECT windowRect);
	Graphics(const Graphics&) = delete;
	~Graphics();

	void ExecuteCommandLists(ID3D12CommandList** commandLists, UINT numCommandLists);
	void Present();
	void PrepareFrame();
	void WaitDeviceIdle();
	void ShowImGui();

	static DXGI_SAMPLE_DESC GetSampleDesc() { return s_SampleDesc; }

private:
	// Function for GaphicsFabric
	static Graphics* GetGraphics() { return s_GraphicsInstance; }

	ComPtr<IDXGISwapChain3> CreateSwapchain(UINT width, UINT height);
	std::vector<ComPtr<ID3D12Resource>> GetSwapchainBuffers(IDXGISwapChain3* swapchain, 
															ComPtr<ID3D12DescriptorHeap>& descriptorHeap, UINT numBuffers,
															UINT& descriptorIncrement) const;
	std::vector<ComPtr<ID3D12Resource>> CreateMultiSampleRenderTargets(UINT width, UINT height, ComPtr<ID3D12DescriptorHeap>& descriptorHeap) const;
	
	void CreateDepthDescriptor(ComPtr<ID3D12Resource> depthBuffer, ComPtr<ID3D12DescriptorHeap>& outDescriptorHeap, DXGI_SAMPLE_DESC sample);

	void Barrier(ComPtr<ID3D12Resource>& res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	void SetRenderTarget(UINT numFrame, ComPtr<ID3D12DescriptorHeap> rtv, ComPtr<ID3D12DescriptorHeap> dsv);

	void PrepareImGuiFrame();
	void RenderImGuiFrame();

public:
	static inline constexpr UINT s_BufferCount = 2;
	static inline const DXGI_FORMAT s_BackBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	static inline const DXGI_FORMAT s_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

private:
	static inline Graphics* s_GraphicsInstance = nullptr;

	UINT m_Width;
	UINT m_Height;
	ComPtr<IDXGIFactory1> m_Factory;
	ComPtr<ID3D12Device1> m_Device;
	ComPtr<IDXGISwapChain3> m_Swapchain;

	ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	std::vector<ComPtr<ID3D12Resource>> m_BackBuffers;
	ComPtr<ID3D12DescriptorHeap> m_RTVMSHeap;
	std::vector<ComPtr<ID3D12Resource>> m_MSBackBuffers;
	UINT m_RTVIncrementSize;
	
	D3D12_CLEAR_VALUE m_RenderTargetClearValue{};

	ComPtr<ID3D12CommandAllocator> m_DirectCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_DirectCommandList;
	ComPtr<ID3D12CommandQueue> m_DirectCommandQueue;
	ComPtr<ID3D12CommandQueue> m_CopyCommandQueue;
	
	Fence m_GraphicsFence;
	Fence m_CopyFence;
	
	D3D12_VIEWPORT m_Viewport{};
	D3D12_RECT m_Scissor{};
	UINT m_BackBufferIndex = 0;
	static inline DXGI_SAMPLE_DESC s_SampleDesc{};

	ComPtr<ID3D12Resource> m_DepthBuffer;
	ComPtr<ID3D12DescriptorHeap> m_DepthDescriptorHeap;

	// ImGuiHeap
	ComPtr<ID3D12DescriptorHeap> m_SrvDescHeap;
};

