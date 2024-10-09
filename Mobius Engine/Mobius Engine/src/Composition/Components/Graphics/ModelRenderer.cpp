#include "ModelRenderer.h"
#include "ImGuiWidgets.h"
#include "Editor/Editor.h"
#include <filesystem>

MeshRenderer::MeshRenderer()
{

}

void MeshRenderer::OnGui()
{
	if (ImGui::BeginCompOnGui("Mesh Renderer"), Editor->mIcons["MeshRenderer"])
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);

		{
		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
		ImGui::TreeNodeEx("From model:", flags);
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);

		ImGui::TreePop();
	}
}

ModelRenderer::ModelRenderer(const std::string& modelPath)
{
	mShader = ResourceMgr->Load<Shader>("data/engine/Shaders/PBR.shader");

	SetModel(modelPath);
}

void ModelRenderer::OnGui()
{
	if (ImGui::BeginCompOnGui("Mesh Renderer"), Editor->mIcons["MeshRenderer"])
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);

		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
			ImGui::TreeNodeEx("Model", flags);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-1);

			static std::string modelActualName = mModel ? mModel->get()->GetFilePath() : "None";
			if (ImGui::BeginCombo("##Model", ResourceMgr->GetResourceName(modelActualName).c_str()))
			{
				std::vector<std::string> engineModelsPath = ResourceMgr->GetAllFilesFrom("data/engine/meshes", ".obj");
				std::vector<std::string> modelsPath = ResourceMgr->GetAllFilesFrom("data/meshes", ".obj");
				modelsPath.insert(modelsPath.begin(), engineModelsPath.begin(), engineModelsPath.end());

				int count = 0;
				for (const std::string& currModelPath : modelsPath)
				{
					if (ImGui::Selectable((ResourceMgr->GetResourceName(currModelPath) + "##" + std::to_string(count)).c_str()))
					{
						//for (auto it : mOwner->mChildren)
						//	delete it;
						//mOwner->mChildren.clear();

						modelActualName = currModelPath;
						mModel = SetModel(currModelPath);
					}

					count++;
				}

				ImGui::EndCombo();
			}
			ImGui::NextColumn();

			ImGui::TreeNodeEx("Shader", flags);
			ImGui::NextColumn();
			ImGui::SetNextItemWidth(-1);

			static std::string shaderActualName = mShader ? mShader->get()->GetFilePath() : "None";
			if (ImGui::BeginCombo("##Shader", ResourceMgr->GetResourceName(shaderActualName).c_str()))
			{
				std::vector<std::string> engineModelsPath = ResourceMgr->GetAllFilesFrom("data/engine/shaders", ".shader");
				std::vector<std::string> modelsPath = ResourceMgr->GetAllFilesFrom("data/shaders", ".shader");
				modelsPath.insert(modelsPath.begin(), engineModelsPath.begin(), engineModelsPath.end());

				int count = 0;
				for (const std::string& currShaderPath : modelsPath)
				{
					if (ImGui::Selectable((ResourceMgr->GetResourceName(currShaderPath) + "##" + std::to_string(count)).c_str()))
					{
						shaderActualName = currShaderPath;
						mShader = ResourceMgr->Load<Shader>(currShaderPath);
					}

					count++;
				}

				ImGui::EndCombo();
			}

			ImGui::NextColumn();
		}

		ImGui::PopStyleVar();
	}
}

Resource<Model>* ModelRenderer::SetModel(const std::string& path)
{
	ClearNodes();

	mModel = ResourceMgr->Load<Model>(path);
	Model* pModel = mModel->get();
	mChildNodes.resize(pModel->mMeshNodes.size());

	for (unsigned i = 0; i < pModel->mMeshNodes.size(); i++)
	{
		Model::MeshNode& currNode = pModel->mMeshNodes[i];

		// Already computing child nodes recursively in CreateNewNodeRecursive 
		if (currNode.mParent != -1)
		{
			SetUpNodes(i, mOwner);
			break;
		}

	}

	return mModel;
}

Resource<Shader>* ModelRenderer::SetShader(const std::string& path)
{
	return nullptr;
}

void ModelRenderer::SetUpNodes(const unsigned nodeIdx, SceneNode* parentNode)
{
	Model* pModel = mModel->get();
	Model::MeshNode& currNode = pModel->mMeshNodes[nodeIdx];
	SceneNode* newNode = new SceneNode(currNode.mName, parentNode);

	for (auto meshIdx : currNode.mMeshIdx)
	{
		if (meshIdx != -1)
		{
			MeshRenderer* comp = newNode->AddComp(new MeshRenderer());
			comp->mModelParent = this;
			comp->mMesh = &pModel->mMeshes[meshIdx];
			comp->mOwner = newNode;
		}
	}

	newNode->transform = currNode.mTransform;
	mChildNodes[nodeIdx] = newNode;

	for (unsigned i = 0; i < currNode.mChildren.size(); i++)
		SetUpNodes(currNode.mChildren[i], newNode);
}

void ModelRenderer::ClearNodes()
{
}

void ModelRenderer::FromJson(json& Val)
{
}

json& ModelRenderer::ToJson(json& Val)
{
	return Val;
}

void ModelRenderer::Render(Shader* shader)
{
	if (!mModel) return;
	Model* pModel = mModel->get();
	Shader* pShader = shader ? shader : mShader->get(); 
	 
	pModel->Render(pShader);

	// Set everything back to defaults once configured for good practice
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(0);
}

/*void ModelRenderer::CreateNewNodeRecursive(const unsigned nodeIdx, SceneNode* parentNode)
{
	Model* pModel = mModel->get();
	GfxNode& currNode = pModel->mNodes[nodeIdx];
	SceneNode* newNode = new SceneNode(currNode.mName, parentNode);
	MeshRenderer* comp = newNode->AddComp(new MeshRenderer());
	comp->mModelParent = this;
	if (currNode.mMeshIdx != -1) comp->mMesh = pModel->mMeshes[currNode.mMeshIdx];
	comp->mOwner = newNode;

	newNode->transform = currNode.mTransform;
	mChildNodes[nodeIdx] = newNode;

	for (unsigned i = 0; i < currNode.mChildren.size(); i++)
		CreateNewNodeRecursive(currNode.mChildren[i], newNode);
}

TResource<Model>* ModelRenderer::SetModel(const char* path)
{
	mModel = ResourceMgr->Load<Model>(path);
	Model* pModel = mModel->get();
	mChildNodes.resize(pModel->mNodes.size());

	for (unsigned i = 0; i < pModel->mNodes.size(); i++)
	{
		GfxNode& currNode = pModel->mNodes[i];

		// Already computing child nodes recursively in CreateNewNodeRecursive 
		if (currNode.mParent != -1) continue;

		CreateNewNodeRecursive(i, mOwner);
	}

	return mModel;
}*/