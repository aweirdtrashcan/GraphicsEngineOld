#pragma once

#include "IDrawable.h"

class Mesh : public IDrawable {
public:
	struct Vertex {
		XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 Normal = { 0.0f, 0.0f, 1.0f };
		XMFLOAT3 TexCoord = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 Tangent = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 Bitangent = { 0.0f, 0.0f, 0.0f };
	};
public:
	Mesh(const std::vector<Mesh::Vertex>& vertex, std::vector<unsigned short>& indices, std::string name, FXMMATRIX transform);

private:
	XMFLOAT4X4 m_Transform;
};