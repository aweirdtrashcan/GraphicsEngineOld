#include "RootSignature.h"

#include "GraphicsFabric.h"
#include <sstream>

#include "BindableCodex.h"

RootSignature::RootSignature(const RootParam& param) {

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 1;
	desc.pParameters = &param.GetD3D12Param();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	
	m_RootSignature = GraphicsFabric::CreateRootSignature(desc);

	std::stringstream ss;
}

RootSignature::RootSignature() {
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 0;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	m_RootSignature = GraphicsFabric::CreateRootSignature(desc);
}

void RootSignature::Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) {
	cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
}

std::shared_ptr<RootSignature> RootSignature::Resolve(const RootParam& param) {
	return BindableCodex::Resolve<RootSignature>(param);
}

std::shared_ptr<RootSignature> RootSignature::Resolve() {
	return BindableCodex::Resolve<RootSignature>();
}

std::string RootSignature::GenerateKey(const RootParam& param) {
	return param.GetName();
}

std::string RootSignature::GenerateKey() {
	return "EmptyRootSignature";
}
