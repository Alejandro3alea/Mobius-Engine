#include "AudioBrowserFiles.h"
#include "Editor.h"

Resource<Texture>* AudioBrowserFile::GetThumbnail()
{
	return Editor->mIcons["AudioFile"];
}

void AudioBrowserFile::OnDragWidget()
{
}

void AudioBrowserFile::OnDropWidget(const ImGuiPayload* payload)
{
}
