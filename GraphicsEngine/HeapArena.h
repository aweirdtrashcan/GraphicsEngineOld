#pragma once

#include "D3DIncls.inl"

class HeapArena {
public:

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateViews(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT size);

	static ID3D12DescriptorHeap*const* GetDescriptorHeap() {
		HeapArena& _this = HeapArena::Get();
		return _this.m_DescriptorHeap.GetAddressOf();
	}

private:
	HeapArena(UINT size, D3D12_DESCRIPTOR_HEAP_TYPE type);
	static HeapArena& Get() {
		static HeapArena arena(1000, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return arena;
	}

	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_DescriptorHeapCPUAddress;
	D3D12_GPU_DESCRIPTOR_HANDLE m_DescriptorHeapGPUAddress;
};

