#include "IndexBuffer.h"

#include "Macros.h"
#include "BindableCodex.h"

#include <sstream>

IndexBuffer::IndexBuffer(const void* indices, SIZE_T indicesCount, 
						 SIZE_T stride, std::string tag)
	:
	m_IndicesCount((UINT)indicesCount) {
	m_Buffer = GraphicsFabric::CreateIndexBuffer(indices, m_IndicesCount, stride, m_View);
}

void IndexBuffer::Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) {
	cmdList->IASetIndexBuffer(&m_View);
}

std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(const void* indices, SIZE_T indicesCount, 
												  SIZE_T stride, std::string tag) {
	return BindableCodex::Resolve<IndexBuffer>(indices, indicesCount, stride, tag);
}

std::string IndexBuffer::GenerateKey(const void* indices, SIZE_T indicesCount, 
									 SIZE_T stride, std::string tag) {
	std::stringstream oss;

	oss << "IndexBuffer#" << indicesCount << stride << "##" << tag;

	return oss.str();
}
