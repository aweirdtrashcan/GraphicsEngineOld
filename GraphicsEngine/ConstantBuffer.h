#pragma once

#include "IBindable.h"

class ConstantBuffer : public IBindable {
public:
	ConstantBuffer(UINT64 structSize, UINT numStructs);

	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

private:
	ComPtr<ID3D12Resource> m_GpuBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	UINT64 m_Stride;
	void* m_MappedData = nullptr;
};

