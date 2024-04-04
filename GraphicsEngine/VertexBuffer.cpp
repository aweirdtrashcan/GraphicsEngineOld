#include "VertexBuffer.h"

#include "GraphicsFabric.h"

#include <sstream>

VertexBuffer::VertexBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, const void* vertices, SIZE_T vertexCount, SIZE_T stride) {
	m_Buffer = GraphicsFabric::CreateVertexBuffer(vertices, vertexCount, stride, m_View);
}

void VertexBuffer::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->IASetVertexBuffers(0, 1, &m_View);
}

std::string VertexBuffer::Resolve(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator, 
								  const void* vertices, SIZE_T vertexCount, SIZE_T stride) {
	std::stringstream oss;

	oss << "VB" << vertexCount << stride;

	return oss.str();
}
