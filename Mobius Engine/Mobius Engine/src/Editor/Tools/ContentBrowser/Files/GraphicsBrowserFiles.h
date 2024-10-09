#pragma once
#include "ContentBrowserFile.h"

struct TextureBrowserFile : public ContentBrowserFile
{
	TextureBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual bool IsDragable() { return true; }
	virtual void OnDragWidget() override;
	virtual void OnDropWidget(const ImGuiPayload* payload) override;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct SkyboxBrowserFile : public ContentBrowserFile
{
	SkyboxBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual void OnDoubleClickTriggered() override;

	virtual bool IsDragable() { return true; }
	virtual void OnDragWidget() override;
	virtual void OnDropWidget(const ImGuiPayload* payload) override;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct CubemapBrowserFile : public ContentBrowserFile
{
	CubemapBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual void OnDoubleClickTriggered() override;

	virtual bool IsDragable() { return true; }
	virtual void OnDragWidget() override;
	virtual void OnDropWidget(const ImGuiPayload* payload) override;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct MaterialBrowserFile : public ContentBrowserFile
{
	MaterialBrowserFile(const std::string& path) : ContentBrowserFile(path) {}

	virtual Resource<Texture>* GetThumbnail() override;

	virtual void OnDoubleClickTriggered() override;

	virtual bool IsDragable() { return true; }
	virtual void OnDragWidget() override;
	virtual void OnDropWidget(const ImGuiPayload* payload) override;
};
