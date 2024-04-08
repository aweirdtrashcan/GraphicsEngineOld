#include "ConstantBuffer.h"

#include "GraphicsFabric.h"
#include "HeapArena.h"

ConstantBuffer::ConstantBuffer(UINT64 structSize, UINT numStructs) 
	: 
	m_Stride(GraphicsFabric::CalculateConstantBufferSize(structSize)) {
	UINT64 cbSize = GraphicsFabric::CalculateConstantBufferSize(structSize) * numStructs;

	m_GpuBuffer = GraphicsFabric::CreateUploadBuffer(nullptr, cbSize);
	m_CpuHandle = HeapArena::CreateViews(m_GpuBuffer->GetGPUVirtualAddress(), cbSize);
	HR_THROW_FAILED(m_GpuBuffer->Map(0, nullptr, &m_MappedData));
}

void ConstantBuffer::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = { m_CpuHandle.ptr + (m_FrameNum * m_Stride) };
}
