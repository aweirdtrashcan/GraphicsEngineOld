#pragma once

#include "IBindable.h"
#include "D3DIncls.inl"
#include <string>

class VertexBuffer : public IBindable {
public:
	VertexBuffer(const void* vertices, SIZE_T vertexCount, SIZE_T stride, std::string key);
	virtual void Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) override;

	static std::shared_ptr<VertexBuffer> Resolve(const void* vertices, SIZE_T vertexCount, 
												 SIZE_T stride, std::string key);

	static std::string GenerateKey(const void* vertices, SIZE_T vertexCount, 
								   SIZE_T stride, std::string key);
private:
	ComPtr<ID3D12Resource> m_Buffer;
	D3D12_VERTEX_BUFFER_VIEW m_View;
};

