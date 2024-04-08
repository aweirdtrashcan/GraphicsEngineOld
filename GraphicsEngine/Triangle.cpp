#include "Triangle.h"

#include "GraphicsFabric.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BindableCodex.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "PrimitiveTopology.h"
#include "ConstantBuffer.h"

#include <DirectXMath.h>

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

	struct CBuf {
		DirectX::XMFLOAT4X4 mvp;

		CBuf() {
			DirectX::XMMATRIX m = DirectX::XMMatrixIdentity();
			DirectX::XMStoreFloat4x4(&mvp, m);
		}
	};

	Vertex v[3] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.5, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f } 
	};

	AddBind(PipelineStateObject::Resolve(PipelineStateObject::Option::MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR));

	auto cb = std::make_shared<ConstantBuffer>(sizeof(CBuf), 2);
	CBuf cbcpu{};
	cb->Update(&cbcpu, sizeof(cbcpu), 0);
	AddBind(cb);

	AddBind(PrimitiveTopology::Resolve(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	std::shared_ptr<VertexBuffer> vBuffer = VertexBuffer::Resolve(
		m_CommandList, 
		v,
		_countof(v),
		sizeof(v[0]),
		"TriangleVB"
	);

	unsigned int i[] = { 0, 1, 2 };

	std::shared_ptr<IndexBuffer> iBuffer = IndexBuffer::Resolve(
		m_CommandList,
		i,
		_countof(i),
		sizeof(i[0]),
		"TriangleIBuf"
	);

	AddBind(std::move(vBuffer));
	AddBind(std::move(iBuffer));
}
