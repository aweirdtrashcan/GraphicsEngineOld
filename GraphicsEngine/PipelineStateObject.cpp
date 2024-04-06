#include "PipelineStateObject.h"

#include "Graphics.h"
#include "GraphicsFabric.h"

PipelineStateObject::PipelineStateObject(const RootSignature& rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
										 const std::vector<Shader>& shaders) {

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

	for (size_t i = 0; i < shaders.size(); i++) {
		switch (shaders[i].GetType()) {
			case Shader::Type::VertexShader:
			{
				desc.VS = CD3DX12_SHADER_BYTECODE(shaders[i].GetBytecode());
				break;
			}
			case Shader::Type::PixelShader:
			{
				desc.PS = CD3DX12_SHADER_BYTECODE(shaders[i].GetBytecode());
				break;
			}
			default: __debugbreak();
		}
	}

	desc.pRootSignature = rootSignature.m_RootSignature.Get();
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.SampleMask = UINT_MAX;
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	desc.RasterizerState.DepthClipEnable = FALSE;
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.DepthStencilState.DepthEnable = FALSE;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.InputLayout.NumElements = static_cast<UINT>(elements.size());
	desc.InputLayout.pInputElementDescs = elements.data();
	desc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	m_PipelineState = GraphicsFabric::CreatePipelineState(desc);
}

void PipelineStateObject::Bind(ID3D12GraphicsCommandList* cmdList) noexcept {
	cmdList->SetPipelineState(m_PipelineState.Get());
}