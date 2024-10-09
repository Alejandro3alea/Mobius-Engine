#include "ImGuiWidgets.h"
#include "Editor.h"

namespace ImGui
{
	const unsigned MaxCharCount = 256;

	bool BeginCompOnGui(const std::string& name, const Resource<Texture>* icon)
	{
		Ensure(!Editor->IsCompBegun(), "EndCompOnGui was not called before initializing another component.");
		Editor->SetCompBegun(true);
		bool Val = Editor->IsCompBegun();

		if (icon)
		{
			bool tHeader = ImGui::CollapsingHeader(("##" + name).c_str());
			{
				GLuint texID = icon->get()->GetID();
				ImGui::SameLine();
				ImGui::Image(reinterpret_cast<ImTextureID>(texID), { 18, 18 }, { 0,1 }, { 1,0 });
				ImGui::SameLine();
				ImGui::Text(name.c_str());
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);

			return tHeader;
		}
		else
		{
			auto Header = ImGui::CollapsingHeader(name.c_str());
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);
			return Header;
		}
	}

	void EndCompOnGui()
	{
		Ensure(Editor->IsCompBegun(), "EndCompOnGui was called with no BeginCompOnGui call beforehand.");
		Editor->SetCompBegun(false);
		bool Val = Editor->IsCompBegun();
		ImGui::Columns(1);
		ImGui::PopStyleVar();
	}
	
	void OnGuiHeader(const std::string& label)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
		ImGui::Columns(2);
		ImGui::TreeNodeEx(label.c_str(), flags);
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1);
	}

	bool OnGuiBase(const bool check, const std::function<void()>& then)
	{
		bool changed = false;
		if (check)
		{
			changed = true;
			then();
		}
		ImGui::NextColumn();

		return changed;
	}

	bool OnGui(const std::string& label, bool& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::Checkbox(("##" + label).c_str(), &val), []() {});
	}

	bool OnGui(const std::string& label, int& val, int min, int max)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::SliderInt(("##" + label).c_str(), &val, min, max), [](){});
	}

	bool OnGui(const std::string& label, unsigned& val, unsigned min, unsigned max)
	{
		OnGuiHeader(label);
		int intVal = static_cast<int>(val);
		return OnGuiBase(ImGui::SliderInt(("##" + label).c_str(), &intVal, min, max), [&val, intVal]()
			{
				val = static_cast<unsigned>(intVal);
			});
	}

	bool OnGui(const std::string& label, short& val)
	{
		OnGuiHeader(label);
		int intVal = static_cast<int>(val);
		return OnGuiBase(ImGui::SliderInt(("##" + label).c_str(), &intVal, 0, 5000), [&val, intVal]()
			{
				val = static_cast<unsigned>(intVal);
			});
	}

	bool OnGui(const std::string& label, long& val)
	{
		OnGuiHeader(label);
		int intVal = static_cast<int>(val);
		return OnGuiBase(ImGui::SliderInt(("##" + label).c_str(), &intVal, 0, 5000), [&val, intVal]()
			{
				val = static_cast<unsigned>(intVal);
			});
	}

	bool OnGui(const std::string& label, float& val, const float rate, const float min, const float max)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragFloat(("##" + label).c_str(), &val, rate, min, max), []() {});
	}

	bool OnGui(const std::string& label, double& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::InputDouble(("##" + label).c_str(), &val), []() {});
	}

	bool OnGui(const std::string& label, std::string& val)
	{
		OnGuiHeader(label);
		char c[MaxCharCount];
		for (unsigned i = 0; i < val.size(); i++)
			c[i] = val[i];
		c[val.size()] = '\0';
		return OnGuiBase(ImGui::InputText(("##" + label).c_str(), c, MaxCharCount), [&val, c]() { val = std::string(c); });
	}

	bool OnGui(const std::string& label, glm::vec4& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragFloat4(("##" + label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::vec3& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragFloat3(("##" + label).c_str(), &val[0], 0.01f), []() {});

	}

	bool OnGui(const std::string& label, glm::vec2& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragFloat2(("##" + label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::uvec4& val)
	{
		OnGuiHeader(label);
		glm::ivec4 intVal = static_cast<glm::ivec4>(val);
		return OnGuiBase(ImGui::DragInt4(("##" + label).c_str(), &intVal[0], 0.01f), [&val, intVal]()
			{
				val = static_cast<glm::uvec4>(intVal);
			});
	}

	bool OnGui(const std::string& label, glm::uvec3& val)
	{
		OnGuiHeader(label);
		glm::ivec3 intVal = static_cast<glm::ivec3>(val);
		return OnGuiBase(ImGui::DragInt3(("##" + label).c_str(), &intVal[0], 0.01f), [&val, intVal]()
			{
				val = static_cast<glm::uvec3>(intVal);
			});
	}

	bool OnGui(const std::string& label, glm::uvec2& val)
	{
		OnGuiHeader(label);
		glm::ivec2 intVal = static_cast<glm::ivec2>(val);
		return OnGuiBase(ImGui::DragInt2(("##" + label).c_str(), &intVal[0], 0.01f), [&val, intVal]()
			{
				val = static_cast<glm::uvec2>(intVal);
			});
	}

	bool OnGui(const std::string& label, glm::ivec4& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragInt4(("##" + label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::ivec3& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragInt4(("##" + label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, glm::ivec2& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragInt4(("##" + label).c_str(), &val[0], 0.01f), []() {});
	}

	bool OnGui(const std::string& label, physx::PxVec3& val)
	{
		OnGuiHeader(label);
		return OnGuiBase(ImGui::DragFloat3(("##" + label).c_str(), &val[0], 0.01f), []() {});
	}
}
