#include "SceneGraph.h"
#include "Cubemap.h"
#include "GfxMgr.h"
#include "Editor.h"
#include "Ensure.h"

SceneGraph* SceneGraph::mpInstance;

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void SceneGraph::NewScene()
{
	if (mRoot.get())
		Clear();
	
	mRoot = std::unique_ptr<SceneNode>(new SceneNode("Root"));
    Editor->mSelectedNode = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void SceneGraph::Clear()
{
	mRoot.reset();
	mRoot = nullptr;
    GfxMgr->mRenderComps.clear();
    GfxMgr->mAlphaRenderComps.clear();

    nodeSpawnIdx = 0;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void SceneGraph::OnGui()
{
	if (ImGui::Begin("SceneGraph"))
	{
        for (auto& child : mRoot->mChildren)
        {
            SceneNode* ptr = Editor->mSelectedNode;
            RecursiveNodeOnGui(child.get());
        }

        if (ImGui::Button("Save")) 
        {
            ImGui::OpenPopup("SavePopup");
        }

        Editor->HandleEditorPopups();
	}

    ImGui::End();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void SceneGraph::FromJson(json& Val)
{
    std::string skyboxPath;
    Val["LevelName"] >> mName;
    Val["Path"] >> mPath;
    Val["Skybox"] >> skyboxPath;
    GfxMgr->SetSkybox(skyboxPath);

    json RootVal = Val["Root"];
    for (auto it = RootVal.begin(); it != RootVal.end(); it++)
    {
        SceneNode& Node = CreateNode();
        Node.FromJson(*it);
    }
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

json& SceneGraph::ToJson(json& Val)
{
    Val["LevelName"] << mName;
    Val["Path"] << mPath;
    Val["Skybox"] << GfxMgr->GetSkybox()->path();

    for (auto it : mRoot->mChildren)
    {
        json ChildVal;
        it->ToJson(ChildVal);
        Val["Root"].push_back(ChildVal);
    }

    return Val;
}

json SceneGraph::CreateNewJson(std::string& filePath)
{
    json Val;
    Val["LevelName"] << ResourceMgr->GetResourceName(filePath);
    Val["Path"] << filePath;
    Val["Skybox"] << GfxMgr->GetSkybox()->path();
    return Val;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

SceneNode& SceneGraph::CreateNode()
{
    std::shared_ptr<SceneNode> node = std::shared_ptr<SceneNode>(new SceneNode("Unnamed (" + std::to_string(nodeSpawnIdx) + ")"));
    mRoot->mChildren.push_back(node);
    nodeSpawnIdx++;
    return *mRoot->mChildren[mRoot->mChildren.size() - 1];
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void SceneGraph::RecursiveNodeOnGui(SceneNode* node)
{
    if (node->mChildren.empty()) // Leaf
    {
        SceneNode* ptr = Editor->mSelectedNode;
        ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (node == ptr)
            leaf_flags |= ImGuiTreeNodeFlags_Selected;

        bool isOpen = ImGui::TreeNodeEx(node->mName.c_str(), leaf_flags);

        if (ImGui::IsItemClicked())
            Editor->mSelectedNode = node;

        // TODO: (BALEA) Parenting
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            ImGui::Text("Parenting to be implemented");
            ImGui::EndDragDropSource();
        }
    }
    else // Node
    {
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (node == Editor->mSelectedNode)
            node_flags |= ImGuiTreeNodeFlags_Selected;

        bool isOpen = ImGui::TreeNodeEx(node->mName.c_str(), node_flags);

        if (ImGui::IsItemClicked())
            Editor->mSelectedNode = node;

        // TODO: (BALEA) Parenting
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
            ImGui::Text("Parenting to be implemented");
            ImGui::EndDragDropSource();
        }

        if (isOpen)
        {
            for (unsigned i = 0; i < node->mChildren.size(); i++)
                RecursiveNodeOnGui(node->mChildren[i].get());

            ImGui::TreePop();
        }
    }
}