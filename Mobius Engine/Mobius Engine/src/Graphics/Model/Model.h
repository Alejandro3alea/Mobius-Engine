#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "ResourceMgr.h"
#include "Material.h"
#include "Transform.h"

#include "../assimp/Importer.hpp"
#include "../assimp/scene.h"
#include "../assimp/postprocess.h"

#include <string>

struct Vertex;
struct Shader;

struct Model
{
	friend struct Mesh;
	friend struct ModelRenderer;
	friend class GraphicsManager;

	struct MeshNode
	{
		std::string mName;
		Transform mTransform;

		int mParent;
		std::vector<int> mChildren;

		std::vector<int> mMeshIdx;
	};

	Model(const std::string& path);

public:
	void Render(Shader* shader);

	std::string GetFilePath() { return mFilePath; }
	std::vector<GLuint> GetVAOs();

	std::vector<MeshNode> GetMeshNodes() { return mMeshNodes; }

private:
	void ProcessModel(const aiScene* scene);
	void ProcessNode(aiNode* node, const aiScene* scene, const int parentIdx);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	Material ProcessMaterial(aiMaterial* mat);
	Resource<Texture>* ProcessTexture(aiMaterial* mat, aiTextureType type);

	Primitive ProcessPrimitive(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

	void ProcessNodeChildren();

private:
	std::string mFilePath;

	std::vector<Mesh> mMeshes;
	std::vector<MeshNode> mMeshNodes;
	std::vector<Material> mMaterials;
};