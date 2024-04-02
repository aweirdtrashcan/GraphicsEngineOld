#include "IndexBuffer.h"

#include "Macros.h"

IndexBuffer::IndexBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator,
						 const void* indices, SIZE_T indicesCount, SIZE_T stride)
	: 
	IBindable(commandList, commandAllocator) {
	GFX_THROW_FAILED(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	m_Buffer = GraphicsFabric::CreateIndexBuffer(m_CommandList, indices, indicesCount, stride, m_View);
}

void IndexBuffer::Bind() noexcept {
	m_CommandList->IASetIndexBuffer(&m_View);
}
