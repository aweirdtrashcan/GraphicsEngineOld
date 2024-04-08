#include "ConstantBuffer.h"

#include "GraphicsFabric.h"
#include "HeapArena.h"

ConstantBuffer::ConstantBuffer(UINT64 structSize, UINT numStructs) 
	: 
	m_Stride(GraphicsFabric::CalculateConstantBufferSize(structSize)) {
	UINT64 cbSize = GraphicsFabric::CalculateConstantBufferSize(structSize) * numStructs;

	m_GpuBuffer = GraphicsFabric::CreateUploadBuffer(nullptr, cbSize);
	m_GpuHandle = HeapArena::CreateViews(m_GpuBuffer->GetGPUVirtualAddress(), (UINT)cbSize);
	HR_THROW_FAILED(m_GpuBuffer->Map(0, nullptr, &m_MappedData));
}

void ConstantBuffer::Bind(ID3D12GraphicsCommandList* cmdList) noexcept(!IS_DEBUG) {
	GFX_THROW_FAILED(cmdList->SetDescriptorHeaps(1, HeapArena::GetDescriptorHeap()));
 	GFX_THROW_FAILED(cmdList->SetGraphicsRootDescriptorTable(0, m_GpuHandle));
}

void ConstantBuffer::Update(const void* data, SIZE_T size, UINT index) noexcept(!IS_DEBUG) {
	char* offsetData = (char*)m_MappedData;
	offsetData += (index * m_Stride);
	memcpy(offsetData, data, size);
}
