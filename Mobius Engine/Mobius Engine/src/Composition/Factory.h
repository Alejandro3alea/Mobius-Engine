#pragma once
#include "Singleton.h"
#include "Rtti.h"
#include "Ensure.h"

#include <map>

struct Serializable;

struct ICreator
{
	virtual Serializable* Create() = 0;
};

template<typename Type>
struct TCreator : public ICreator
{
	Serializable * Create() override
	{
		return dynamic_cast<Serializable*>(new Type);
	}
};

class FactoryManager
{
	Singleton(FactoryManager);

	void Initialize();
	
	template <typename T>
	T * Create()
	{
		Rtti type(typeid(T));
		std::string typeName = type.GetRttiType();
		Ensure(mCreators.find(typeName) != mCreators.end(), "No specified creator found in the ICreator list");

		return dynamic_cast<T*>(mCreators[typeName]->Create());
	}

	Serializable * Create(const std::string & objName)
	{
		Ensure(mCreators.find(objName) != mCreators.end(), "No specified creator found in the ICreator list");

		return mCreators[objName]->Create();
	}

	template <typename T>
	void RegisterCreator()
	{
		Rtti type(typeid(T));
		std::string typeName = type.GetRttiType();
		if (mCreators.find(typeName) != mCreators.end()) // Creator already added
			return;

		mCreators[typeName] = new TCreator<T>;
	}

private:
	std::map<std::string, ICreator*> mCreators;
};

#define Factory FactoryManager::Instance()