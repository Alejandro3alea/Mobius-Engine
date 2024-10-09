#pragma once
#include "Misc/Singleton.h"

#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <map>

struct IResourceBase
{
	virtual ~IResourceBase() {}
};

template <typename T>
struct Resource : public IResourceBase
{
	Resource(const std::shared_ptr<T>& obj, const std::string& path) : mResource(obj), mPath(path) {}

	operator T() const { return *mResource.get(); }
	T* get() const { return mResource.get(); }

	std::string path() { return mPath; }

private:
	std::string mPath;
	std::shared_ptr<T> mResource;
};


struct IResourceImporterBase
{
	~IResourceImporterBase();

	virtual IResourceBase* Import(const std::string& path) = 0;
};

template <typename T>
struct ResourceImporter : public IResourceImporterBase
{
	virtual IResourceBase* Import(const std::string& path) override
	{
		return new Resource<T>(std::shared_ptr<T>(new T(path)), path);
	}

	template <class... Ts>
	IResourceBase* Import(Ts... constructorParams)
	{
		return new Resource<T>(std::shared_ptr<T>(new T(constructorParams)), constructorParams...);
	}
};


class ResourceManager
{
	Singleton(ResourceManager);

public:
	void Initialize();
	void Shutdown();

	template <typename T>
	Resource<T>* Load(const std::string& path);

	const std::string GetExtension(const std::string& filePath) const;
	const std::string GetResourceName(const std::string& filePath, const bool getWithExtension = true) const;
	const std::vector<std::string> GetAllFilesFrom(const std::string& folderPath, const std::string& extension = "", const bool recursiveSearch = true) const;

	void ReloadShaders();

public:
	std::map<std::string, std::shared_ptr<IResourceImporterBase>> mAllImporters;
	std::map<std::string, std::shared_ptr<IResourceBase>> mAllResources;
};

#define ResourceMgr ResourceManager::Instance()

template<typename T>
inline Resource<T>* ResourceManager::Load(const std::string& path)
{
	const std::string ext = GetExtension(path);

	if (mAllImporters.find(ext) == mAllImporters.end())
	{
		std::cout << "[WARNING] There is no proper extension for \"." << ext << "\" files yet." << std::endl;
		return nullptr;
	}

	if (mAllResources.find(path) != mAllResources.end())
		return  dynamic_cast<Resource<T>*>(mAllResources[path].get());

	Resource<T>* resource = dynamic_cast<Resource<T>*>(mAllImporters[ext].get()->Import(path));
	mAllResources[path] = std::shared_ptr<IResourceBase>(resource);

	return resource;
}