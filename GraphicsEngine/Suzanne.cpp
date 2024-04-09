#include "Suzanne.h"

#include "PipelineStateObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "PrimitiveTopology.h"

#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

Suzanne::Suzanne(const char* modelPath) {
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
		| aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	
	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 bitangent;
		XMFLOAT3 texCoord;
	};
	
	aiMesh* mesh = scene->mMeshes[0];

	Vertex* vertices = new Vertex[mesh->mNumVertices];
	memset(vertices, 0, sizeof(Vertex) * mesh->mNumVertices);
	UINT* indices = new UINT[mesh->mNumFaces * 3];

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		memcpy(&vertices[i], &mesh->mVertices[i], sizeof(aiVector3D) * 4);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, sizeof(indices[0]) * 3);
	}

	std::shared_ptr<VertexBuffer> vBuffer = VertexBuffer::Resolve(vertices, mesh->mNumVertices, sizeof(Vertex), "suzannevb");
	std::shared_ptr<IndexBuffer> iBuffer = IndexBuffer::Resolve(indices, mesh->mNumFaces * 3, sizeof(UINT), "suzanneib");

	AddBind(PipelineStateObject::Resolve(PipelineStateObject::Option::MVP_DESCRIPTOR_TABLE_PS__POS_NORM_TAN_BITAN__VS__POS_NORM_COORD_TAN_BITAN));
	AddBind(vBuffer);
	AddBind(iBuffer);
	AddBind(PrimitiveTopology::Resolve(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<ConstantBuffer>(sizeof(CBuf), Graphics::s_BufferCount));
	
	delete[] vertices;
	delete[] indices;
}
