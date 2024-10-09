#pragma once
#include "Singleton.h"

#include "PxPhysicsAPI.h"

#include <vector>

using namespace physx;

struct RigidBody;

class PhysicsManager
{
	Singleton(PhysicsManager)

	void Initialize();
	void Update();
	void Shutdown();

	void AddRigidBody(RigidBody* RB);
	void RemoveRigidBody(RigidBody* RB);

	void AddActor(PxRigidActor* Actor);
	void RemoveActor(PxRigidActor*& Actor);

	PxGeometry* CreateGeometry();
	PxMaterial* CreateMaterial();

	PxPhysics* SDK() { return mPhysics; }
	PxScene* GetScene() { return mScene;	}

private:
	PxFoundation* mFoundation = nullptr;
	PxDefaultErrorCallback mDefaultErrorCallback;
	PxDefaultAllocator mDefaultAllocatorCallback;

	PxPvd*		mPvd		= nullptr;
	PxPhysics*	mPhysics	= nullptr;
	PxScene*	mScene		= nullptr;

	//Optional?
	PxDefaultCpuDispatcher* mDispatcher;

	std::vector<RigidBody*> mRigidBodies;
};

#define PhysicsMgr PhysicsManager::Instance()