#pragma once

#include "IBindable.h"
#include "D3DIncls.inl"
#include "GraphicsFabric.h"

class IndexBuffer : public IBindable {
public:
	IndexBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator,
				const void* indices, SIZE_T indicesCount, SIZE_T stride);
	virtual void Bind() noexcept override;
	
private:
	ComPtr<ID3D12Resource> m_Buffer;
	D3D12_INDEX_BUFFER_VIEW m_View{};
};
