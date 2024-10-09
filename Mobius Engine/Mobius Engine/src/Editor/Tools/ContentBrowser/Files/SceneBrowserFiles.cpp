#include "SceneBrowserFiles.h"
#include "Editor.h"

Resource<Texture>* SceneBrowserFile::GetThumbnail()
{
	return Editor->mIcons["MapFile"];
}

void SceneBrowserFile::OnDoubleClickTriggered()
{
	json Val = FileToJson(mPath);
	Scene->NewScene();
	Scene->FromJson(Val);
}

void SceneBrowserFile::OnDragWidget()
{
	ImGui::SetDragDropPayload("ITEM_PAYLOAD", &mPath, sizeof(mPath));
	ImGui::Text("Dragging: %s", mFileName.c_str());
}

void SceneBrowserFile::OnDropWidget(const ImGuiPayload* payload)
{
	// Is from another ContentBrowserFile
	if (payload)
	{
		std::string* filePath = (std::string*)payload->Data;
	}
	else if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) // Is in main scene
	{
		json Val = FileToJson(mPath);
		Scene->NewScene();
		Scene->FromJson(Val);
	}
}