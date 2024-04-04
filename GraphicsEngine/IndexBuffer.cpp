#include "IndexBuffer.h"

#include "Macros.h"
#include <sstream>

IndexBuffer::IndexBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, const void* indices, SIZE_T indicesCount, SIZE_T stride)
	:
	m_IndicesCount((UINT)indicesCount) {
	m_Buffer = GraphicsFabric::CreateIndexBuffer(indices, m_IndicesCount, stride, m_View);
}

void IndexBuffer::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->IASetIndexBuffer(&m_View);
}

std::string IndexBuffer::Resolve(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator,
								 const void* indices, SIZE_T indicesCount, SIZE_T stride) {
	std::stringstream oss;

	oss << "IB" << indicesCount << stride;

	return oss.str();
}
