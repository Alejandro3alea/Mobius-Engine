#include "Model.h"
#include "Shader/Shader.h"
#include "Misc/ColorConsole.h"

#include "../assimp/Importer.hpp"
#include "../assimp/scene.h"
#include "../assimp/postprocess.h"

#include <iostream>

Model::Model(const std::string& path) : mFilePath(path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		PrintError("Error loading model " + path + ": " + importer.GetErrorString());
		return;
	}
	ProcessModel(scene);
}

void Model::ProcessModel(const aiScene* scene)
{
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		mMaterials.push_back(ProcessMaterial(scene->mMaterials[i]));

	int node = -1;
	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, -1);
	ProcessNodeChildren();
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, const int parentIdx)
{
	int nodeIdx = mMeshNodes.size();
	MeshNode currNode;
	currNode.mParent = parentIdx;
	currNode.mName = node->mName.C_Str();

	aiVector3D pos, sca;
	aiQuaternion rot;
	node->mTransformation.Decompose(sca, rot, pos);
	currNode.mTransform = Transform({ pos.x, pos.y, pos.z }, { sca.x, sca.y, sca.z }, { rot.x, rot.y, rot.z, rot.w });

	
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		currNode.mMeshIdx.push_back(mMeshes.size());
		mMeshes.push_back(ProcessMesh(mesh, scene));
	}

	mMeshNodes.push_back(currNode);

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene, nodeIdx);
}

struct Vertex
{
	Vertex() = default;
	Vertex(const glm::vec3& _pos, const glm::vec2& _uv, const glm::vec3& _normal) :
		pos(_pos), uv(_uv), normal(_normal) {}

	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	Mesh finalMesh;

	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Resource<Texture>> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = vec;
		}
		else
			vertex.uv = glm::vec2(0.0f, 0.0f);

		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		// tangent
		if (mesh->mTangents)
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;
		}

		// bitangent
		if (mesh->mBitangents)
		{
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}


		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	Primitive prim = ProcessPrimitive(vertices, indices);
	prim.mMatIdx = mesh->mMaterialIndex;
	
	finalMesh.mPrimitives.push_back(prim);
	finalMesh.mModel = this;

	return finalMesh;
}

Material Model::ProcessMaterial(aiMaterial* mat)
{
	Material finalMat;
	finalMat.mAlbedoTex	   = ProcessTexture(mat, aiTextureType_DIFFUSE);
	finalMat.mNormalTex    = ProcessTexture(mat, aiTextureType_HEIGHT);
	finalMat.mMetallicTex  = ProcessTexture(mat, aiTextureType_SPECULAR);
	finalMat.mRoughnessTex = ProcessTexture(mat, aiTextureType_UNKNOWN);
	finalMat.mOclussionTex = ProcessTexture(mat, aiTextureType_LIGHTMAP);
	finalMat.mEmissiveTex  = ProcessTexture(mat, aiTextureType_EMISSIVE);
	return finalMat;
}

Resource<Texture>* Model::ProcessTexture(aiMaterial* mat, aiTextureType type)
{
	unsigned texCount = mat->GetTextureCount(type);

	if (texCount == 0)
		return nullptr;

	aiString str;
	aiString path("data");
	mat->GetTexture(type, 0, &str);
	path.Append(str.C_Str());
	return ResourceMgr->Load<Texture>(path.C_Str());
}

Primitive Model::ProcessPrimitive(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	Primitive prim;
	GLuint VBO;
	glGenVertexArrays(1, &prim.mVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &prim.mEBO);

	glBindVertexArray(prim.mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim.mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex texture uvs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	// vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	//glDeleteBuffers(1, &VBO);

	glBindVertexArray(0);

	prim.mUsingIndices = !indices.empty();
	prim.mCount = prim.mUsingIndices ? indices.size() : vertices.size();
	return prim;
}

void Model::ProcessNodeChildren()
{
	for (unsigned i = 1; i < mMeshNodes.size(); i++)
	{
		int parent = mMeshNodes[i].mParent;
		mMeshNodes[parent].mChildren.push_back(i);
	}
}

void Model::Render(Shader* shader)
{
	shader->Bind();

	for (unsigned int i = 0; i < mMeshNodes.size(); i++)
	{
		Model::MeshNode& currNode = mMeshNodes[i];

		for (unsigned int j = 0; j < currNode.mMeshIdx.size(); j++)
			mMeshes[currNode.mMeshIdx[j]].Render(shader);
	}
}

std::vector<GLuint> Model::GetVAOs()
{
	std::vector<GLuint> VAOlist;
	for (auto mesh : mMeshes)
		for (auto primitive : mesh.mPrimitives)
			VAOlist.push_back(primitive.mVAO);

	return VAOlist;
}
