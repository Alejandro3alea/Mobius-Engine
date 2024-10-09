#pragma once
#include "ResourceMgr.h"
#include "Texture.h"

#include "imgui/imgui.h"

#include <string>

// Default case: Unknown file type
struct ContentBrowserFile
{
	ContentBrowserFile(const std::string& path);

public:
	virtual Resource<Texture>* GetThumbnail();

	virtual void OnDoubleClickTriggered();

	virtual bool IsDragable() { return false; }
	virtual void OnDragWidget() {}
	virtual void OnDropWidget(const ImGuiPayload* payload) {}

	const std::string mPath;
	std::string mFileName;
};