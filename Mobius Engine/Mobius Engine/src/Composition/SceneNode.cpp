#include "SceneNode.h"
#include "Editor/Editor.h"
#include "Composition/Rtti.h"
#include "Composition/Factory.h"
#include "Editor/ImGuiWidgets.h"

// Gizmos
#include "GfxMgr.h"
#include "WindowMgr.h"
#include "InputMgr.h"
#include "imgui/ImGuizmo.h"

#include "Components/Audio/AudioEmitter.h"
#include "Components/Physics/RigidBody.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const unsigned MaxCharCount = 256;

SceneNode::~SceneNode()
{
	PrintDebug("Deleted: " + mName);

	for (auto it : mComps)
	{
		delete it;
	}
}

void SceneNode::FromJson(json& Val)
{
	Val[".Name"] >> mName;

	transform.FromJson(Val["transform"]);

	if (mParent)
		worldTransform.Concatenate(mParent->worldTransform);
	else
		worldTransform = transform;

	json compsVal = Val["Comps"];
	for (auto jsonIt = compsVal.begin(); jsonIt != compsVal.end(); jsonIt++)
	{
		json compVal = *jsonIt;
		Component* Comp = dynamic_cast<Component*>(Factory->Create(compVal[".Rtti"]));
		AddComp(Comp);
		Comp->FromJson(compVal);
	}

	json childrenVal = Val["Children"];
	for (auto jsonIt = childrenVal.begin(); jsonIt != childrenVal.end(); jsonIt++)
	{
		SceneNode& ChildNode = Scene->CreateNode();
		ChildNode.mParent = this;
		ChildNode.FromJson(*jsonIt);
		mChildren.push_back(std::shared_ptr<SceneNode>(&ChildNode));
	}
}

json& SceneNode::ToJson(json& Val)
{
	Val[".Name"] << mName;

	transform.ToJson(Val["transform"]);

	for (auto Comp : mComps)
	{
		json CompVal;
		Rtti compRtti(*Comp);
		CompVal[".Rtti"] = compRtti.GetRttiType();
		Comp->ToJson(CompVal);
		Val["Comps"].push_back(CompVal);
	}

	for (auto Child : mChildren)
	{
		json ChildVal;
		Child->ToJson(ChildVal);
		Val["Children"].push_back(ChildVal);
	}

	return Val;
}

void SceneNode::OnGui()
{
	if (ImGui::Begin("Node Editor"))
	{
		char c[MaxCharCount];
		for (unsigned i = 0; i < mName.size(); i++)
			c[i] = mName[i];
		c[mName.size()] = '\0';
		if (ImGui::InputText("Name", c, MaxCharCount))
			mName = std::string(c);

		ImGui::SameLine();
		ImGui::Dummy({ 5.0f, 0.0f });
		ImGui::SameLine();
        if (ImGui::Button("+Add", { 60.0f, 0.0f }))
            ImGui::OpenPopup("AddPopup");

		if (ImGui::BeginPopup("AddPopup"))
        {
			if (ImGui::MenuItem("RigidBody"))
				AddComp(new RigidBody());
			if (ImGui::MenuItem("Audio Source"))
				AddComp(new AudioEmitter());

            ImGui::EndPopup();
        }

		ImGui::Separator();

		transform.OnGui();
		ImGui::Separator();

		for (auto it : mComps)
		{
			it->OnGui();
			ImGui::EndCompOnGui();
			ImGui::Separator();
		}
	}

	ImGui::End();


	// Gizmos
	const glm::uvec2 winSize = WindowMgr->mCurrentWindow->size();
	ImGuizmo::SetRect(0, 0, winSize.x, winSize.y);

	if (InputMgr->isKeyDown(SDL_SCANCODE_1))
		Editor->mGizmoMode = ImGuizmo::TRANSLATE;
	else if (InputMgr->isKeyDown(SDL_SCANCODE_2))
		Editor->mGizmoMode = ImGuizmo::SCALE;
	else if (InputMgr->isKeyDown(SDL_SCANCODE_3))
		Editor->mGizmoMode = ImGuizmo::ROTATE;


	const glm::mat4& proj = Editor->mEditorCam.GetProjMtx();
	const glm::mat4& view = Editor->mEditorCam.GetViewMtx();
	glm::mat4 model = worldTransform.GetModelMtx();

	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), static_cast<ImGuizmo::OPERATION>(Editor->mGizmoMode), ImGuizmo::LOCAL, glm::value_ptr(model));
	if (ImGuizmo::IsUsing())
	{
		Transform t;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::mat4 invMat(1.0f);
		switch (static_cast<ImGuizmo::OPERATION>(Editor->mGizmoMode))
		{
		case ImGuizmo::OPERATION::TRANSLATE:
			glm::decompose(model, t.scale, t.rotation, t.pos, skew, perspective);
			transform.pos += (t.pos - worldTransform.pos);
			break;
		case ImGuizmo::OPERATION::SCALE:
			invMat *= glm::mat4_cast(glm::inverse(worldTransform.rotation));
			invMat = glm::translate(invMat, glm::vec3(-worldTransform.pos));
			glm::decompose(invMat * model, t.scale, t.rotation, t.pos, skew, perspective);
			transform.scale += t.scale - worldTransform.scale;
			break;
		case ImGuizmo::OPERATION::ROTATE:
			invMat = glm::translate(invMat, glm::vec3(worldTransform.pos));
			invMat = glm::scale(invMat, glm::vec3(worldTransform.scale));
			glm::decompose(invMat * model, t.scale, t.rotation, t.pos, skew, perspective);
			transform.rotation *= glm::inverse(worldTransform.rotation) * t.rotation;
			break;
		}
	}
}