#include "HeapArena.h"

#include "GraphicsFabric.h"

HeapArena::HeapArena(UINT size, D3D12_DESCRIPTOR_HEAP_TYPE type) {
	m_DescriptorHeap = GraphicsFabric::CreateDescriptorHeap(size, type);
	m_DescriptorHeapUsage = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE HeapArena::CreateViews(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT size) {

	HeapArena& _this = Get();

	GraphicsFabric::CreateConstantBufferView(bufferLocation, size, _this.m_DescriptorHeapUsage);

	D3D12_CPU_DESCRIPTOR_HANDLE viewHandle = _this.m_DescriptorHeapUsage;

	_this.m_DescriptorHeapUsage.ptr += size;

	return viewHandle;
}
