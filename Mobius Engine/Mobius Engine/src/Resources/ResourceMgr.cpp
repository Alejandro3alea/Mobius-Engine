#include "ResourceMgr.h"
#include "Shader/Shader.h"
#include "Shader/ComputeShader.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Model/Model.h"
#include "Audio/Sound.h"

#include "Misc/ColorConsole.h"

#include <iostream>
#include <filesystem>


ResourceManager* ResourceManager::mpInstance;

void ResourceManager::Initialize()
{
	mAllImporters["shader"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Shader>());
	mAllImporters["comp"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<ComputeShader>());
	mAllImporters["png"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["jpg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["jpeg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["obj"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Model>());
	mAllImporters["fbx"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Model>());
	mAllImporters["cubemap"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Cubemap>());
	mAllImporters["hdr"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Skybox>());
	mAllImporters["wav"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Sound>());
}

void ResourceManager::Shutdown()
{
	mAllResources.clear();
	mAllImporters.clear();
}

const std::string ResourceManager::GetExtension(const std::string& filePath) const
{
	size_t pos = filePath.find_last_of('.');
	if (pos == std::string::npos)
		return "";

	return filePath.substr(pos + 1);
}

const std::string ResourceManager::GetResourceName(const std::string& filePath, const bool getWithExtension) const
{
	size_t extensionLoc = filePath.find_last_of('.');
	size_t directoryLoc = filePath.find_last_of('\\');

	if (directoryLoc == std::string::npos)
		directoryLoc = filePath.find_last_of('/');

	size_t start = (directoryLoc != std::string::npos) ? directoryLoc + 1 : 0;
	size_t end = (extensionLoc != std::string::npos && !getWithExtension) ? extensionLoc : filePath.length();

	std::string result;
	for (size_t i = start; i < end; i++)
		result.push_back(filePath[i]);

	return result;
}

const std::vector<std::string> ResourceManager::GetAllFilesFrom(const std::string& folderPath, const std::string& extension, const bool recursiveSearch) const
{
	std::vector<std::string> fileList;
	std::filesystem::path shaderDir = std::filesystem::path(folderPath);

	int count = 0;
	for (auto dirIt = std::filesystem::recursive_directory_iterator(shaderDir);
		dirIt != std::filesystem::recursive_directory_iterator();
		dirIt++, ++count)
	{
		if (std::filesystem::is_directory(dirIt->path()))
		{
			if (recursiveSearch)
			{
				std::vector<std::string> subfolderFiles = GetAllFilesFrom(dirIt->path().string(), extension, true);
				fileList.insert(fileList.end(), subfolderFiles.begin(), subfolderFiles.end());
			}
			else
				continue;
		}

		std::string fileExtension = dirIt->path().extension().string();
		if (extension != "" && fileExtension != extension)
			continue;

		fileList.push_back(dirIt->path().string());
	}

	return fileList;
}

void ResourceManager::ReloadShaders()
{
	const std::shared_ptr<IResourceImporterBase> Importer = mAllImporters["shader"];

	for (auto& CurrResource : mAllResources)
	{
		const std::string Path = CurrResource.first;
		const std::string Ext = GetExtension(Path);

		if (Ext == "shader")
		{
			Shader NewShader(Path);
			Shader* CurrShader = dynamic_cast<Resource<Shader>*>(CurrResource.second.get())->get();
			std::swap(NewShader.mID, CurrShader->mID);
		}
	}
}

IResourceImporterBase::~IResourceImporterBase()
{
	PrintDebug("Deleted Resource Importer.");
}