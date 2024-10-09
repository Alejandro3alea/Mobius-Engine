#include "ContentBrowserFileHandler.h"
#include "ContentBrowser.h"
#include "Editor.h"

#include "Files/GraphicsBrowserFiles.h"
#include "Files/AudioBrowserFiles.h"
#include "Files/FolderBrowserFile.h"
#include "Files/SceneBrowserFiles.h"


ContentBrowserFile* ContentBrowserFileHandler::GetFile(const std::filesystem::directory_entry& file)
{
	const std::string Filename = file.path().filename().string();
	const std::string PathStr = ContentBrowser->GetPath() + "/" + Filename;
	bool IsRegularFile = file.is_regular_file();

	if (!IsRegularFile) // It's a folder!
		return new FolderBrowserFile(PathStr);

	const std::string Extension = ResourceMgr->GetExtension(Filename);

	if (Extension == "png" || Extension == "jpg")
		return new TextureBrowserFile(PathStr);

	if (Extension == "level")
		return new SceneBrowserFile(PathStr);

	if (Extension == "mat")
		return new MaterialBrowserFile(PathStr);

	if (Extension == "mp3" || Extension == "wav" || Extension == "ogg")
		return new AudioBrowserFile(PathStr);

	if (Extension == "skybox")
		return new CubemapBrowserFile(PathStr);

	if (Extension == "hdr")
		return new SkyboxBrowserFile(PathStr);

	return new ContentBrowserFile(PathStr);
}