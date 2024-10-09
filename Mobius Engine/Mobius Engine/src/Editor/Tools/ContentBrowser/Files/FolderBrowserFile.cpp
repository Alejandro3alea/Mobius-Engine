#include "FolderBrowserFile.h"
#include "Editor/Tools/ContentBrowser/ContentBrowser.h"
#include "Editor.h"

Resource<Texture>* FolderBrowserFile::GetThumbnail()
{
	return Editor->mIcons["Folder"];
}

void FolderBrowserFile::OnDoubleClickTriggered()
{
    std::string fileName = mPath.substr(ContentBrowser->GetPath().size());
	ContentBrowser->UpdatePath(ContentBrowser->GetPath() + fileName);
}