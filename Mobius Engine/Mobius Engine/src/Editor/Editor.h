#pragma once
#include "Singleton.h"
#include "SceneGraph.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "Camera.h"
#include "Events/Event.h"
#include "Editor/Tools/EditorTool.h"
#include "Misc/Ensure.h"

#include "imgui/imgui.h"

#include <map>

struct SceneNode;

CREATE_EVENT(OnDragWidget);
CREATE_MULTICAST_EVENT(OnDropWidget, void*);

#undef LoadIcon
class ImGuiEditor
{
	Singleton(ImGuiEditor);

	friend struct SceneNode;
	friend struct Component;
	friend struct Transform;

public:
	void Initialize();
	void Load();
	void Update();
	void Shutdown();

	void Pause();
	void Stop();
	void Play();
	bool IsPlaying();

	void Dockspace();
	void MainMenu();

	void ShowPostProcessSettings();

	void HandleEditorPopups();

	Resource<Texture>* GetIcon(const std::string& iconName);

	bool IsCompBegun() { return mIsCompBegun; }
	void SetCompBegun(const bool IsCompBegun) { mIsCompBegun = IsCompBegun; }

	void SetModalFlagEnabled(const std::string& modalName);
	void SetDragAndDropPayload(void* payload, const unsigned size);

private:
	void LoadIcon(const std::string& name);

public:
	bool mbDockWindowOpen;
	ImGuiDockNodeFlags mDockspaceFlags = ImGuiDockNodeFlags_None;

	SceneNode* mSelectedNode = nullptr;

	unsigned long selectionMask = 0;

	EditorCamera mEditorCam;

	OnDragWidget mOnDragWidget;
	OnDropWidget mOnDropWidget;

public: // for now
	std::map<std::string, Resource<Texture>*> mIcons;

private:
	bool mIsPlaying = false;
	unsigned mGizmoMode = 0;

	void* mDragAndDropPayload = nullptr;

	std::vector<EditorTool*> mTools;

	// Variable used for component creation
	bool mIsCompBegun = false;

	bool mShowPostProcessSettings = false;

	std::string mEditText = "";

	std::map<std::string, bool> mWindowOpenChecks;
	std::map<std::string, bool> mModalChecks;
};

#define Editor ImGuiEditor::Instance()