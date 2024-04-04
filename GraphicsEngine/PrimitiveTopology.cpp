#include "PrimitiveTopology.h"

PrimitiveTopology::PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) : m_Topology(topology) {}

void PrimitiveTopology::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->IASetPrimitiveTopology(m_Topology);
}
