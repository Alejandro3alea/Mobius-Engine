#pragma once
#include "ContentBrowserFile.h"

struct SceneBrowserFile : public ContentBrowserFile
{
	SceneBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual void OnDoubleClickTriggered() override;

	virtual bool IsDragable() { return true; }
	virtual void OnDragWidget() override;
	virtual void OnDropWidget(const ImGuiPayload* payload) override;
};