#include "Triangle.h"

#include "GraphicsFabric.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BindableCodex.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "PrimitiveTopology.h"

Triangle::Triangle() {
	m_CommandAllocator = GraphicsFabric::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE);
	m_CommandList = GraphicsFabric::CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_CommandAllocator);

	struct Vertex {
		struct {
			float x, y, z;
		};
		struct {
			float r, g, b;
		};
	};

	Vertex v[3] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.5, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f } 
	};

	std::shared_ptr<VertexBuffer> vBuffer = std::make_shared<VertexBuffer>(
		m_CommandList, 
		v,
		_countof(v),
		sizeof(v[0])
	);

	unsigned int i[] = { 0, 1, 2 };

	std::shared_ptr<IndexBuffer> iBuffer = std::make_shared<IndexBuffer>(
		m_CommandList,
		i,
		_countof(i),
		sizeof(i[0])
	);

	AddBind(std::move(vBuffer));
	AddBind(std::move(iBuffer));
	
	std::shared_ptr<RootSignature> rootSig = std::make_shared<RootSignature>();
	
	std::vector<D3D12_INPUT_ELEMENT_DESC> ied = {
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	std::vector<Shader> shaders;
	shaders.push_back({ L"Shaders/VertexShader.cso", Shader::Type::VertexShader });
	shaders.push_back({ L"Shaders/PixelShader.cso", Shader::Type::PixelShader });

	std::shared_ptr<PipelineStateObject> pipeline = std::make_shared<PipelineStateObject>(
		*rootSig.get(),
		ied,
		shaders
	);

	AddBind(std::make_shared<PrimitiveTopology>(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::move(rootSig));
	AddBind(std::move(pipeline));
}
