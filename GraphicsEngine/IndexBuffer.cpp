#include "IndexBuffer.h"

#include "Macros.h"
#include "BindableCodex.h"

#include <sstream>

IndexBuffer::IndexBuffer(ComPtr<ID3D12GraphicsCommandList> commandList, const void* indices, SIZE_T indicesCount, 
						 SIZE_T stride, std::string tag)
	:
	m_IndicesCount((UINT)indicesCount) {
	m_Buffer = GraphicsFabric::CreateIndexBuffer(indices, m_IndicesCount, stride, m_View);
}

void IndexBuffer::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->IASetIndexBuffer(&m_View);
}

std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(ComPtr<ID3D12GraphicsCommandList> commandList, const void* indices, 
												  SIZE_T indicesCount, SIZE_T stride, std::string tag) {
	return BindableCodex::Resolve<IndexBuffer>(commandList, indices, indicesCount, stride, tag);
}

std::string IndexBuffer::GenerateKey(ComPtr<ID3D12GraphicsCommandList> commandList, const void* indices, 
									 SIZE_T indicesCount, SIZE_T stride, std::string tag) {
	std::stringstream oss;

	oss << "IndexBuffer#" << indicesCount << stride << "##" << tag;

	return oss.str();
}
