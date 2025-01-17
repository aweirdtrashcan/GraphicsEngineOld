#pragma once

#include "IBindable.h"

class PrimitiveTopology : public IBindable {
public:
	PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
	virtual void Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) override;

	static std::shared_ptr<PrimitiveTopology> Resolve(D3D12_PRIMITIVE_TOPOLOGY topology);
	static std::string GenerateKey(D3D12_PRIMITIVE_TOPOLOGY topology);

private:
	D3D12_PRIMITIVE_TOPOLOGY m_Topology;
};

