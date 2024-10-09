#pragma once
#include "ResourceMgr.h"
#include "Texture.h"
#include "Audio/Sound.h"
#include "Physics/PhysicsMgr.h"

#include <string>

namespace ImGui
{
	bool BeginCompOnGui(const std::string& name, const Resource<Texture>* icon = nullptr);
	void EndCompOnGui();

	void OnGuiHeader(const std::string& label);

	bool OnGui(const std::string& label, bool& val);
	bool OnGui(const std::string& label, int& val, int min = -INT_MIN / 3, int max = INT_MAX/ 3);
	bool OnGui(const std::string& label, unsigned& val, unsigned min = 0, unsigned max = INT_MAX / 3);
	bool OnGui(const std::string& label, short& val);
	bool OnGui(const std::string& label, long& val);
	bool OnGui(const std::string& label, float& val, const float rate = 0.01f, const float min = -FLT_MAX, const float max = FLT_MAX);
	bool OnGui(const std::string& label, double& val);

	bool OnGui(const std::string& label, std::string& val);

	bool OnGui(const std::string& label, glm::vec4& val);
	bool OnGui(const std::string& label, glm::vec3& val);
	bool OnGui(const std::string& label, glm::vec2& val);
	bool OnGui(const std::string& label, glm::uvec4& val);
	bool OnGui(const std::string& label, glm::uvec3& val);
	bool OnGui(const std::string& label, glm::uvec2& val);
	bool OnGui(const std::string& label, glm::ivec4& val);
	bool OnGui(const std::string& label, glm::ivec3& val);
	bool OnGui(const std::string& label, glm::ivec2& val);
	bool OnGui(const std::string& label, physx::PxVec3& val);

	//template<typename T>
	//bool OnGui(const std::string& label, const std::map<std::string, std::shared_ptr<T>>& resourceList);


}

namespace ImGui
{
	/*template<typename T>
	bool OnGui(const std::string& label, const std::map<std::string, std::shared_ptr<Sound>>& resourceList)
	{
		std::map<std::string, std::shared_ptr<Sound>> monoSounds, stereoSounds, otherSounds;
		for (auto it : resourceList)
		{
			switch (it.second->GetNumChannels())
			{
			case 1:
				monoSounds.push_back(it);
				break;

			case 2:
				stereoSounds.push_back(it);
				break;

			default:
				otherSounds.push_back(it);
				break;
			}
		}

		// Using the generic BeginCombo() API, you have full control over how to display the combo contents.
		// (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
		// stored in the object itself, etc.)
		const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
		static int item_current_idx = 0;                    // Here our selection data is an index.
		const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically could be anything)(
		if (ImGui::BeginCombo("combo 1", combo_label, flags))
		{
			ImGui::Text("Mono");
			for (auto it : monoSounds)
			{
				if (ImGui::Selectable(it.first.c_str()))
				{

				}
			}
			/*for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(monoSounds[n], is_selected))
					item_current_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		return false;
	}*/
}