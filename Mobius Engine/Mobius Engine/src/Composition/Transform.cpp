#include "Transform.h"
#include "Editor/Editor.h"
#include "Editor/ImGuiWidgets.h"

glm::mat4 Transform::GetModelMtx()
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
	model = model * glm::toMat4(rotation);
	model = glm::scale(model, glm::vec3(scale));
	return model;
}

Transform& Transform::Concatenate(const Transform& rhs)
{
	scale *= rhs.scale;
	rotation = rhs.rotation * rotation;
	pos = rhs.rotation * (rhs.scale * pos) + rhs.pos;

	return *this;
}

Transform& Transform::InvConcatenate(const Transform& rhs)
{
	scale /= rhs.scale;
	rotation = glm::inverse(rhs.rotation) * rotation;
	pos = (1.0f / rhs.scale) * (glm::inverse(rhs.rotation) * (pos - rhs.pos));

	return *this;
}

const Transform Transform::operator+(const Transform& rhs)
{
	return Concatenate(rhs);
}

const Transform Transform::operator-(const Transform& rhs)
{
	return InvConcatenate(rhs);
}

void Transform::OnGui()
{
	if (ImGui::BeginCompOnGui("Transform", Editor->mIcons["Transform"]))
	{
		ImGui::OnGui("Translation", pos);
		ImGui::OnGui("Scale", scale);
		//ImGui::OnGui("Rotation", rotation);
	}

	ImGui::EndCompOnGui();
}

void Transform::FromJson(json& Val)
{
	Val["pos"] >> pos;
	Val["scale"] >> scale;
	//@TODO
	//Val["rotation"] >> rotation; 
}

json& Transform::ToJson(json& Val)
{
	Val["pos"] << pos;
	Val["scale"] << scale;
	//@TODO
	//Val["rotation"] << rotation; 

	return Val;
}
