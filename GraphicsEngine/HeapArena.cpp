#include "HeapArena.h"

#include "GraphicsFabric.h"

HeapArena::HeapArena(UINT size, D3D12_DESCRIPTOR_HEAP_TYPE type) {
	m_DescriptorHeap = GraphicsFabric::CreateDescriptorHeap(size, type);
	m_DescriptorHeapCPUAddress = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_DescriptorHeapGPUAddress = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE HeapArena::CreateViews(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT size) {

	HeapArena& _this = Get();

	GraphicsFabric::CreateConstantBufferView(bufferLocation, size, _this.m_DescriptorHeapCPUAddress);

	D3D12_GPU_DESCRIPTOR_HANDLE viewHandle = _this.m_DescriptorHeapGPUAddress;

	UINT64 incrementSize = (UINT64)GraphicsFabric::GetDescriptorIncrement(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_this.m_DescriptorHeapCPUAddress.ptr += incrementSize;
	_this.m_DescriptorHeapGPUAddress.ptr += incrementSize;

	return viewHandle;
}
