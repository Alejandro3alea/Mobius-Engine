#pragma once
#include "ContentBrowserFile.h"

struct AudioBrowserFile : public ContentBrowserFile
{
	AudioBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual bool IsDragable() { return true; }
	virtual void OnDragWidget() override;
	virtual void OnDropWidget(const ImGuiPayload* payload) override;
};