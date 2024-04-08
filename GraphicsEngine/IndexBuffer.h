#pragma once

#include "IBindable.h"
#include "D3DIncls.inl"
#include "GraphicsFabric.h"

class IndexBuffer : public IBindable {
public:
	IndexBuffer(const void* indices, SIZE_T indicesCount, SIZE_T stride, std::string tag);
	virtual void Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) override;
	
	static std::shared_ptr<IndexBuffer> Resolve(const void* indices, SIZE_T indicesCount, SIZE_T stride, std::string tag);
	static std::string GenerateKey(const void* indices, SIZE_T indicesCount, SIZE_T stride, std::string tag);

	UINT GetIndicesCount() const { return m_IndicesCount; }

private:
	ComPtr<ID3D12Resource> m_Buffer;
	D3D12_INDEX_BUFFER_VIEW m_View{};
	UINT m_IndicesCount;
};
