#include "ConstantBuffer.h"

#include "GraphicsFabric.h"
#include "HeapArena.h"

ConstantBuffer::ConstantBuffer(UINT64 structSize, UINT numStructs, UINT registerSlot)
	: 
	m_Stride(GraphicsFabric::CalculateConstantBufferSize(structSize)),
	m_Register(registerSlot) {
	UINT64 cbSize = GraphicsFabric::CalculateConstantBufferSize(structSize) * numStructs;

	m_GpuBuffer = GraphicsFabric::CreateUploadBuffer(nullptr, cbSize);
	m_GpuHandle.ptr = 0;
	for (UINT i = 0; i < numStructs; i++) {
		// Keep track only of the first index pointer. The reason is because on Bind we do the math
		// to offset the pointer depending on the frame number.
		if (m_GpuHandle.ptr == 0) {
			m_GpuHandle = HeapArena::CreateViews(m_GpuBuffer->GetGPUVirtualAddress(), static_cast<UINT>(cbSize / numStructs));
		} else {
			HeapArena::CreateViews(m_GpuBuffer->GetGPUVirtualAddress(), static_cast<UINT>(cbSize / numStructs));
		}
	}
	HR_THROW_FAILED(m_GpuBuffer->Map(0, nullptr, &m_MappedData));
}

void ConstantBuffer::Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuAddr = m_GpuHandle;
	gpuAddr.ptr += (frameNumber * GraphicsFabric::GetDescriptorIncrement(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	GFX_THROW_FAILED(cmdList->SetDescriptorHeaps(1, HeapArena::GetDescriptorHeap()));
 	GFX_THROW_FAILED(cmdList->SetGraphicsRootDescriptorTable(m_Register, gpuAddr));
}

void ConstantBuffer::Update(const void* data, SIZE_T size, UINT index) noexcept(!IS_DEBUG) {
	char* offsetData = (char*)m_MappedData;
	offsetData += (index * m_Stride);
	memcpy(offsetData, data, size);
}
