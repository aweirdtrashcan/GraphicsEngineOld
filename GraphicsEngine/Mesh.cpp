#include "Mesh.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <memory>

Mesh::Mesh(const std::vector<Mesh::Vertex>& vertex, std::vector<unsigned short>& indices, std::string name, FXMMATRIX transform) {
	std::shared_ptr<VertexBuffer> vBuffer = VertexBuffer::Resolve(vertex.data(), vertex.size(), sizeof(Mesh::Vertex), name);
	std::shared_ptr<IndexBuffer> iBuffer = IndexBuffer::Resolve(indices.data(), indices.size(), sizeof(unsigned short), name);


}
