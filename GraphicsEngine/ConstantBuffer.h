#pragma once

#include "IBindable.h"

class ConstantBuffer : public IBindable {
public:
	ConstantBuffer(UINT64 structSize, UINT numStructs, UINT registerSlot = 0);

	virtual void Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) override;
	void Update(const void* data, SIZE_T size, UINT index) noexcept(!IS_DEBUG);

private:
	ComPtr<ID3D12Resource> m_GpuBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
	UINT64 m_Stride;
	UINT m_Register;
	void* m_MappedData = nullptr;
};

