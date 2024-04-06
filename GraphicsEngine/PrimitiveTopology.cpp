#include "PrimitiveTopology.h"

#include "BindableCodex.h"

PrimitiveTopology::PrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology) : m_Topology(topology) {}

void PrimitiveTopology::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->IASetPrimitiveTopology(m_Topology);
}

std::shared_ptr<PrimitiveTopology> PrimitiveTopology::Resolve(D3D12_PRIMITIVE_TOPOLOGY topology) {
	return BindableCodex::Resolve<PrimitiveTopology>(topology);
}

std::string PrimitiveTopology::GenerateKey(D3D12_PRIMITIVE_TOPOLOGY topology) {
	return std::string(std::to_string((UINT)topology));
}
