#pragma once

#include "IBindable.h"

class PrimitiveTopology : public IBindable {
public:
	PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

private:
	D3D12_PRIMITIVE_TOPOLOGY m_Topology;
};

