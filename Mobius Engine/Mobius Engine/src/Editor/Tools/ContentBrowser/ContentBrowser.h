#pragma once
#include "Editor/Tools/EditorTool.h"
#include "Files/ContentBrowserFile.h"
#include "Events/Event.h"

#include <string>

CREATE_MULTICAST_EVENT(OnContentBrowserUpdate, std::string);

class ContentBrowserTool : public EditorTool
{
	Singleton(ContentBrowserTool)

	virtual void Initialize() final;
	virtual void Update() final;

	const std::string GetPath();
	void UpdatePath(const std::string& path);
	
private:
	void DrawHeader();

	void UpdateFiles();

public:
	std::vector<ContentBrowserFile*> mCurrPathFiles;

	OnContentBrowserUpdate mOnContentBrowserUpdate;

private:
	std::string mCurrPath;
};

#define ContentBrowser ContentBrowserTool::Instance()