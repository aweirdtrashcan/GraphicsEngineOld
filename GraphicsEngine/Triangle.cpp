#include "Triangle.h"

#include "GraphicsFabric.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BindableCodex.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "PrimitiveTopology.h"
#include "ConstantBuffer.h"

Triangle::Triangle() {
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

	AddBind(PipelineStateObject::Resolve(PipelineStateObject::Option::MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR));

	auto cb = std::make_shared<ConstantBuffer>(sizeof(CBuf), Graphics::s_BufferCount);
	cb->Update(&m_CBuf, sizeof(m_CBuf), 0);
	m_ConstantBuffer = cb.get();
	AddBind(cb);

	AddBind(PrimitiveTopology::Resolve(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	std::shared_ptr<VertexBuffer> vBuffer = VertexBuffer::Resolve(
		v,
		_countof(v),
		sizeof(v[0]),
		"TriangleVB"
	);

	unsigned int i[] = { 0, 1, 2 };

	std::shared_ptr<IndexBuffer> iBuffer = IndexBuffer::Resolve(
		i,
		_countof(i),
		sizeof(i[0]),
		"TriangleIBuf"
	);

	AddBind(std::move(vBuffer));
	AddBind(std::move(iBuffer));
}

void Triangle::Rotate(float x, float y, float z, UINT frameNum) {
	XMMATRIX m = XMLoadFloat4x4(&m_CBuf.mvp) * XMMatrixRotationRollPitchYaw(x, y, z);
	XMFLOAT4X4 data;
	XMStoreFloat4x4(&data, XMMatrixTranspose(m));
	m_ConstantBuffer->Update(&data, sizeof(data), frameNum);
}
