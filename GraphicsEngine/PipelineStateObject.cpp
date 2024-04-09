#include "PipelineStateObject.h"

#include "Graphics.h"
#include "GraphicsFabric.h"
#include "BindableCodex.h"

#include <sstream>
#include <cassert>

PipelineStateObject::PipelineStateObject(ComPtr<ID3D12RootSignature> rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
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

	desc.pRootSignature = rootSignature.Get();
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
	desc.RTVFormats[0] = Graphics::s_BackBufferFormat;
	desc.SampleDesc = Graphics::GetSampleDesc();

	m_PipelineState = GraphicsFabric::CreatePipelineState(desc);
	m_RootSignature = rootSignature;
}

void PipelineStateObject::Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) {
	cmdList->SetPipelineState(m_PipelineState.Get());
	cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
}

std::shared_ptr<PipelineStateObject> PipelineStateObject::Resolve(PipelineStateObject::Option option) {
	if (m_PipelineCaches.empty()) {
		BuildAllPipelineStates();
	}
	return m_PipelineCaches[option];
}

void PipelineStateObject::BuildAllPipelineStates() {
	std::vector<Shader> shaders;
	shaders.push_back({ L"Shaders/VertexShader.cso", Shader::Type::VertexShader });
	shaders.push_back({ L"Shaders/PixelShader.cso", Shader::Type::PixelShader });

	std::vector<D3D12_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	Build_EMPTY_ROOT_SIG_PS__COLOR_VS__POS_COLOR(ied, shaders);

	shaders[0] = Shader(L"Shaders/VertexShaderMVP.cso", Shader::Type::VertexShader);

	D3D12_ROOT_SIGNATURE_DESC rootDesc{};
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_DESCRIPTOR_RANGE range[2]{};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[0].NumDescriptors = 1;
	range[0].BaseShaderRegister = 0;

	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[1].NumDescriptors = 1;
	range[1].BaseShaderRegister = 1;

	D3D12_ROOT_PARAMETER params[2]{};
	params[0].DescriptorTable.NumDescriptorRanges = 1;
	params[0].DescriptorTable.pDescriptorRanges = &range[0];
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	params[1].DescriptorTable.NumDescriptorRanges = 1;
	params[1].DescriptorTable.pDescriptorRanges = &range[1];
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	rootDesc.NumParameters = _countof(params);
	rootDesc.pParameters = params;

	ComPtr<ID3D12RootSignature> rootSignature = GraphicsFabric::CreateRootSignature(rootDesc);

	Build_MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR(ied, shaders, rootSignature.Get());
	Build_MVP_DESCRIPTOR_TABLE_PS__POS_NORM_TAN_BITAN__VS__POS_NORM_COORD_TAN_BITAN(rootSignature.Get());
}

void PipelineStateObject::Build_EMPTY_ROOT_SIG_PS__COLOR_VS__POS_COLOR(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied,
																	   const std::vector<Shader>& shaders) {
	std::shared_ptr<PipelineStateObject>& op = m_PipelineCaches[Option::EMPTY_ROOT_SIG_PS__COLOR_VS__POS_COLOR];
	assert(op == nullptr);

	D3D12_DESCRIPTOR_RANGE range{};
	range.BaseShaderRegister = 1;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range.NumDescriptors = 1;

	D3D12_ROOT_PARAMETER param{};
	param.DescriptorTable.NumDescriptorRanges = 1;
	param.DescriptorTable.pDescriptorRanges = &range;
	param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

	D3D12_ROOT_SIGNATURE_DESC rootDesc{};
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	rootDesc.NumParameters = 1;
	rootDesc.pParameters = &param;

	ComPtr<ID3D12RootSignature> rootSignature = GraphicsFabric::CreateRootSignature(rootDesc);

	op = std::make_shared<PipelineStateObject>(rootSignature, ied, shaders);
}

void PipelineStateObject::Build_MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied, 
																			 const std::vector<Shader>& shaders,
																			 ID3D12RootSignature* rootSignature) {
	std::shared_ptr<PipelineStateObject>& op = m_PipelineCaches[Option::MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR];
	assert(op == nullptr);

	op = std::make_shared<PipelineStateObject>(rootSignature, ied, shaders);
}

void PipelineStateObject::Build_MVP_DESCRIPTOR_TABLE_PS__POS_NORM_TAN_BITAN__VS__POS_NORM_COORD_TAN_BITAN(ID3D12RootSignature* rootSignature) {
	std::shared_ptr<PipelineStateObject>& op = m_PipelineCaches[Option::MVP_DESCRIPTOR_TABLE_PS__POS_NORM_TAN_BITAN__VS__POS_NORM_COORD_TAN_BITAN];
	assert(op == nullptr);

	std::vector<D3D12_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TexCoord", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "Bitangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	std::vector<Shader> shaders = {
		Shader(L"Shaders/VertexShaderPNCTB.cso", Shader::Type::VertexShader),
		Shader(L"Shaders/PixelShaderPNCTB.cso", Shader::Type::PixelShader)
	};

	op = std::make_shared<PipelineStateObject>(rootSignature, ied, shaders);
}
