#include "ContentBrowserFile.h"
#include "Editor.h"

#include <filesystem>

ContentBrowserFile::ContentBrowserFile(const std::string& path) : mPath(path)
{
	std::filesystem::directory_entry file(path);
	mFileName = file.path().filename().string();
}

Resource<Texture>* ContentBrowserFile::GetThumbnail()
{
	return Editor->mIcons["Unknown"];
}

void ContentBrowserFile::OnDoubleClickTriggered()
{
	const std::string CommandStr = "open";

	const std::wstring Command(CommandStr.begin(), CommandStr.end());
	const std::wstring FullPath(mPath.begin(), mPath.end());
	HINSTANCE result = ShellExecute(NULL, Command.c_str(), FullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	if ((int)result > 32)
	{
		// File opened successfully
	}
	else
	{
		// Error opening the file
	}
}