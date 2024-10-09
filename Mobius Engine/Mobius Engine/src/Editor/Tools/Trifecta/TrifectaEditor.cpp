#include "TrifectaEditor.h"
#include "Editor/imgui/imnodes.h"
#include "Input/InputMgr.h"

#include <iostream>
#include <fstream>

TrifectaEditor* TrifectaEditor::mpInstance;

void TrifectaEditor::Initialize()
{
	mbIsEnabled = false;

	imnodes::Initialize();

	mAttribIdx = mEdgeIdx = 0;

	mNodes.push_back(new OutputNode);
}

void TrifectaEditor::Update()
{
	for (auto it : mNodes)
		it->isRendered = false;

	for (auto it : mNodes)
		if (!it->isRendered)
			it->DrawShader();

	ImGui::Begin("Trifecta Editor");
	imnodes::BeginNodeEditor();

	for (unsigned i = 0; i < mNodes.size(); i++)
		mNodes[i]->OnGui(i);

	for (const auto& it : mEdges)
		imnodes::Link(it.first, it.second.from, it.second.to);

	//if (imnodes::IsEditorHovered())
	{
		if (!ImGui::IsAnyItemHovered() && InputMgr->isButtonPressed(InputMgr->eButtonRight))
			ImGui::OpenPopup("Add node");


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
		if (ImGui::BeginPopup("Add node"))
		{
			const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

			ImGui::Text("Add new node:");

			if (ImGui::MenuItem("Add (1)"))
				mNodes.push_back(new AddFloatNode);
			if (ImGui::MenuItem("Substract (1)"))
				mNodes.push_back(new SubstractFloatNode);
			if (ImGui::MenuItem("Add (3)"))
				mNodes.push_back(new AddVec3Node);
			if (ImGui::MenuItem("Substract (3)"))
				mNodes.push_back(new SubstractVec3Node);
			if (ImGui::MenuItem("Perlin Noise"))
				mNodes.push_back(new PerlinNoiseNode);
			if (ImGui::MenuItem("Float to Vec3"))
				mNodes.push_back(new FloatToVec3);
			if (ImGui::MenuItem("Time"))
				mNodes.push_back(new TimeNode);

			if (ImGui::MenuItem("Process Shader"))
				ProcessShader();

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}

	imnodes::EndNodeEditor();

	{
		int from, to;
		if (imnodes::IsLinkCreated(&from, &to))
		{
			NodeAttribute* toAttribute = GetInputAttribute(to);
			NodeAttribute* fromAttribute = GetOutputAttribute(from);

			if (fromAttribute->type == toAttribute->type)
			{
				fromAttribute->linkIdx = toAttribute->linkIdx = mEdgeIdx;

				mEdges.insert(std::make_pair(GetEdgeIdx(), NodeEdge(from, to)));

				reinterpret_cast<ShaderNode*>(toAttribute->parentNode)->UpdatePersonalShader();
			}
		}
	}

	int linkIdx;
	if (imnodes::IsLinkDestroyed(&linkIdx))
	{
		NodeAttribute* fromAttribute = GetInputAttribute(mEdges.find(linkIdx)->second.from);
		NodeAttribute* toAttribute   = GetOutputAttribute(mEdges.find(linkIdx)->second.to);

		fromAttribute->linkIdx = toAttribute->linkIdx = -1;
		reinterpret_cast<ShaderNode*>(toAttribute->parentNode)->UpdatePersonalShader();

		mEdges.erase(linkIdx);
	}

	const int linkCount = imnodes::NumSelectedLinks();
	if (linkCount > 0 && InputMgr->isKeyDown(SDL_SCANCODE_DELETE))
	{
		std::vector<int> selectedLinks(linkCount);
		imnodes::GetSelectedLinks(selectedLinks.data());
		for (const int currIdx : selectedLinks)
		{
			auto linkData = mEdges.find(currIdx);
			if (linkData == mEdges.end())
				continue;

			NodeAttribute* fromAttribute = GetOutputAttribute(linkData->second.from);
			NodeAttribute* toAttribute = GetInputAttribute(linkData->second.to);

			fromAttribute->linkIdx = toAttribute->linkIdx = -1;
			reinterpret_cast<ShaderNode*>(toAttribute->parentNode)->UpdatePersonalShader();

			mEdges.erase(currIdx);
		}
	}

	ImGui::End();
}

void TrifectaEditor::Shutdown()
{
	for (unsigned i = 0; i < mNodes.size(); i++)
		delete mNodes[i];

	mNodes.clear();
}

void TrifectaEditor::ProcessShader()
{
	ProcessVertex();
	ProcessFragment();
}

void TrifectaEditor::DrawNodes()
{
}

void TrifectaEditor::ProcessVertex()
{
	std::ofstream result("SampleShader.vert");

	assert(result.is_open());

	result << "#version 400 core\n" << std::endl;

	result << "layout (location = 0) in vec3 vPos;" << std::endl;
	result << "layout (location = 1) in vec2 vTexCoord;" << std::endl;
	result << "layout (location = 2) in vec3 vNormal;" << std::endl;
	result << "layout (location = 3) in vec3 vTangent;" << std::endl;
	result << "layout (location = 4) in vec3 vBitangent;\n" << std::endl;

	result << "void main()" << std::endl;
	result << "{" << std::endl;

	dynamic_cast<OutputNode*>(mNodes[0])->WriteVertex(result);

	result << "}" << std::endl;

	result.close();
}

void TrifectaEditor::ProcessFragment()
{
	std::ofstream result("SampleShader.frag");

	assert(result.is_open());

	result << "#version 400 core\n" << std::endl;

	result << "void main()" << std::endl;
	result << "{" << std::endl;

	dynamic_cast<OutputNode*>(mNodes[0])->WriteFragment(result);

	result << "}" << std::endl;

	result.close();
}

NodeAttribute* TrifectaEditor::GetInputAttribute(const unsigned idx)
{
	for (auto it : mNodes)
		for (auto it2 : it->mInParams)
			if (it2->id == idx)
				return it2;

	return nullptr;
}

NodeAttribute* TrifectaEditor::GetOutputAttribute(const unsigned idx)
{
	for (auto iter : mNodes)
		for (auto iter2 : iter->mOutParams)
			if (iter2->id == idx)
				return iter2;

	return nullptr;
}