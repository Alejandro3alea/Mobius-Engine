#include "PhysicsMgr.h"
#include "Components/Physics/RigidBody.h"
#include "Timer.h"

#define PVD_HOST "127.0.0.1"

PhysicsManager* PhysicsManager::mpInstance;

void PhysicsManager::Initialize()
{
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
    if (!mFoundation)
        abort();  //fatalError("PxCreateFoundation failed!");

    bool recordMemoryAllocations = true;

    mPvd = PxCreatePvd(*mFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);


    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mPvd);
    if (!mPhysics)
        abort(); //fatalError("PxCreatePhysics failed!");

    mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    PxSceneDesc SceneDesc(mPhysics->getTolerancesScale());
    SceneDesc.gravity = { 0.0f, -9.81f, 0.0f };
    SceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    SceneDesc.cpuDispatcher = mDispatcher;
    SceneDesc.filterShader = PxDefaultSimulationFilterShader;
    //SceneDesc.simulationEventCallback = &myCollisionEventCallback;  // Asigna un objeto que implemente la interfaz PxSimulationEventCallback
    mScene = mPhysics->createScene(SceneDesc);

    // Optional (Check Startup & Shutdown)
    /*mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(scale));
    if (!mCooking)
        fatalError("PxCreateCooking failed!");*/
}

void PhysicsManager::Update()
{
    for (auto it : mRigidBodies)
        it->BeginUpdate();

    const float dt = TimeMgr->deltaTime;
    mScene->simulate(dt);
    mScene->fetchResults(true);

    for (auto it : mRigidBodies)
        it->EndUpdate();
}

void PhysicsManager::Shutdown()
{
    mScene->release();
    mPhysics->release();
    mFoundation->release();
}

void PhysicsManager::AddRigidBody(RigidBody* RB)
{
    AddActor(RB->GetActor());
    mRigidBodies.push_back(RB);
}

void PhysicsManager::RemoveRigidBody(RigidBody* RB)
{
    RemoveActor(RB->GetActor());

    auto at = std::find(mRigidBodies.begin(), mRigidBodies.end(), RB);
    mRigidBodies.erase(at);
}

void PhysicsManager::AddActor(PxRigidActor* Actor)
{
    mScene->addActor(*Actor);
}

void PhysicsManager::RemoveActor(PxRigidActor*& Actor)
{
    mScene->removeActor(*Actor);
    Actor->release();
    Actor = nullptr;
}
