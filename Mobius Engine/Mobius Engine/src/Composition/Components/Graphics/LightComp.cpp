#include "LightComp.h"
#include "Editor.h"
#include "ImGuiWidgets.h"
#include "GfxMgr.h"

LightComp::LightComp()
{
	GfxMgr->mLights.push_back(this);
}

LightComp::~LightComp()
{
	std::remove(GfxMgr->mLights.begin(), GfxMgr->mLights.end(), this);
}

void LightComp::OnGui()
{
	if (ImGui::BeginCompOnGui("Light", Editor->mIcons["Light"]))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);

		{
			ImGui::OnGui("Color", mColor);
		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();
	}
}

void LightComp::FromJson(json& Val)
{
	Val["color"] >> mColor;
}

json& LightComp::ToJson(json& Val)
{
	Val["color"] << mColor;
	return Val;
}
