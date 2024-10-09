#pragma once
#include "ContentBrowserFile.h"

// Folder type
struct FolderBrowserFile : public ContentBrowserFile
{
	FolderBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual void OnDoubleClickTriggered() override;
};
