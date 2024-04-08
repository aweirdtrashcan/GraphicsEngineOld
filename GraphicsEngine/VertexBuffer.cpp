#include "VertexBuffer.h"

#include "GraphicsFabric.h"
#include "BindableCodex.h"

#include <sstream>

VertexBuffer::VertexBuffer(const void* vertices, SIZE_T vertexCount, 
						   SIZE_T stride, std::string key) {
	m_Buffer = GraphicsFabric::CreateVertexBuffer(vertices, vertexCount, stride, m_View);
}

void VertexBuffer::Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) {
	cmdList->IASetVertexBuffers(0, 1, &m_View);
}

std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(const void* vertices, SIZE_T vertexCount, 
													SIZE_T stride, std::string key) {
	return BindableCodex::Resolve<VertexBuffer>(vertices, vertexCount, stride, key);
}

std::string VertexBuffer::GenerateKey(const void* vertices, SIZE_T vertexCount, 
									  SIZE_T stride, std::string key) {
	std::stringstream oss;

	oss << "VB" << vertexCount << stride << key;

	return oss.str();
}
