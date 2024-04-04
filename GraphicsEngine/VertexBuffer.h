#pragma once

#include "IBindable.h"
#include "D3DIncls.inl"

class VertexBuffer : public IBindable {
public:
	VertexBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, const void* vertices, SIZE_T vertexCount, SIZE_T stride);
	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

	static std::string Resolve(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator,
							   const void* vertices, SIZE_T vertexCount, SIZE_T stride);
private:
	ComPtr<ID3D12Resource> m_Buffer;
	D3D12_VERTEX_BUFFER_VIEW m_View;
};

