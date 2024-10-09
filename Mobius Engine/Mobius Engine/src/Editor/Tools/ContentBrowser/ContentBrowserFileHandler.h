#pragma once
#include "Files/ContentBrowserFile.h"
#include "ResourceMgr.h"
#include "Texture.h"

#include <filesystem>

class ContentBrowserFileHandler
{
public:
	static ContentBrowserFile* GetFile(const std::filesystem::directory_entry& file);
};