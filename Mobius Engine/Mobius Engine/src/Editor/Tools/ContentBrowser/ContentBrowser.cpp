#include "ContentBrowser.h"
#include "ContentBrowserFileHandler.h"
#include "Editor.h"

#include "imgui/ImCurveEdit.h"
#include "imgui/imgui_internal.h"

#include <filesystem>
#include <iostream>
#include <Windows.h>

ContentBrowserTool* ContentBrowserTool::mpInstance;

void ContentBrowserTool::Initialize()
{
	mbIsEnabled = true;
	mOnContentBrowserUpdate.Add([this](const std::string& path) { UpdateFiles(); });

	UpdatePath(PROJECT_DIR + std::string("data"));
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

bool Thumbnail(ContentBrowserFile* file, const ImVec2& size, const char* label) noexcept 
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	//If the window is collapsed
	if (window->SkipItems)
		return false;

	Texture* Texture = file->GetThumbnail()->get();
	ImTextureID texID = reinterpret_cast<ImTextureID>(Texture->GetID());
	std::string buttonID = "#" + file->mPath;
	ImGui::ImageButton(buttonID.c_str(), texID, size, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			Editor->SetDragAndDropPayload(file, sizeof(ContentBrowserFile));
			file->OnDragWidget();
			Editor->mOnDropWidget.Clear();
			Editor->mOnDropWidget.Add([file](void* payload) { file->OnDropWidget(nullptr); });
			ImGui::EndDragDropSource();
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_PAYLOAD"))
			file->OnDropWidget(payload);

		ImGui::EndDragDropTarget();
	}
	bool isPressed = false;
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		isPressed = true;


	ImGui::TextWrapped("%s", file->mFileName.c_str());

	ImGui::NextColumn();

	return isPressed;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ContentBrowserTool::Update()
{
	ImGui::Begin("Content Browser");

	DrawHeader();
	
	ImGuiStyle& style = ImGui::GetStyle();
	const ImVec2 buttonSize = ImVec2(120.0f, 120.0f);
	float windowSizeX = ImGui::GetContentRegionAvail().x;
	const unsigned collumnCount = max(static_cast<unsigned>(windowSizeX / (buttonSize.x + style.ItemSpacing.x * 2.0f)), 1);

	ImGui::Columns(collumnCount, nullptr, false);

    for (const auto& currFile : mCurrPathFiles)
    {
		if (Thumbnail(currFile, buttonSize, currFile->mFileName.c_str()))
		{
			currFile->OnDoubleClickTriggered();
			break;
		}
    }

	ImGui::Columns(1);

	if (ImGui::BeginPopupContextWindow("ContextMenu", ImGuiMouseButton_Right, false)) 
	{
		ImGui::MenuItem("Content Browser", NULL, false, false);
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				Editor->SetModalFlagEnabled("CreateFolder");
			}

			if (ImGui::MenuItem("Level"))
			{
				Editor->SetModalFlagEnabled("CreateLevel");
			}

			if (ImGui::MenuItem("Material"))
			{
				Editor->SetModalFlagEnabled("CreateMaterial");
			}

			ImGui::EndMenu();
		}
		
		ImGui::EndPopup();
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

const std::string ContentBrowserTool::GetPath()
{
	return mCurrPath;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ContentBrowserTool::UpdatePath(const std::string& path)
{
	mCurrPath = path;
	std::replace(mCurrPath.begin(), mCurrPath.end(), '\\', '/');
	mOnContentBrowserUpdate.Broadcast(path);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ContentBrowserTool::DrawHeader()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

	std::stringstream ss(mCurrPath);
	std::string CurrItem;
	std::vector<std::string> Paths;

	while (std::getline(ss, CurrItem, '/')) 
		Paths.push_back(CurrItem);

	std::string FullPath;
	for (unsigned i = 0; i < Paths.size(); i++)
	{
		if (i != 0)
		{
			ImGui::Text(">");
			ImGui::SameLine();
		}

		const std::string& CurrPath = Paths[i];
		FullPath += CurrPath + "/";
		if (ImGui::Button(CurrPath.c_str()))
		{
			FullPath.pop_back();
			UpdatePath(FullPath);
		}

		ImGui::SameLine();
	}

	ImGui::Text("");
	ImGui::PopStyleColor(1);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void ContentBrowserTool::UpdateFiles()
{
	for (const auto& file : mCurrPathFiles)
		delete file;

	mCurrPathFiles.clear();

	for (const auto& entry : std::filesystem::directory_iterator(mCurrPath))
		mCurrPathFiles.push_back(ContentBrowserFileHandler::GetFile(entry));
}
