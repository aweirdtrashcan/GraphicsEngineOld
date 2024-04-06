#include "RootSignature.h"

#include "GraphicsFabric.h"

RootSignature::RootSignature() {

	D3D12_ROOT_PARAMETER params[2] = {};

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 0;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	
	m_RootSignature = GraphicsFabric::CreateRootSignature(desc);
}

void RootSignature::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
}

const char* RootSignature::Serialize() const noexcept {
	return "not used yet";
}
