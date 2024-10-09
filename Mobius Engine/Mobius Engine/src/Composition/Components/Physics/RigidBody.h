#pragma once
#include "Component.h"
#include "Physics/PhysicsMgr.h"
#include "Physics/PhysicsCollider.h"

enum class PhxBodyStateType
{
	eDynamic,
	eStatic
};

enum class PhxColliderType
{
	eBoxCollider,
	eSphereCollider
};

struct RigidBody : public Component
{
	RigidBody();
	~RigidBody();

	virtual void Initialize() override;

	void BeginUpdate();
	void EndUpdate();

	virtual void OnGui() override;

	virtual void FromJson(json& Val) override;
	virtual json& ToJson(json& Val) override;

	void ChangeCollider(const PhxColliderType ColliderType);
	void ChangeState();

	PxRigidActor*& GetActor() { return mActor; }

private:
	void UpdatePhysicsData();
	void ReleasePhysicsData();

private:
	PhxColliderType mColliderType = PhxColliderType::eBoxCollider;
	Collider* mCollider = nullptr;
	PxShape* mShape = nullptr;
	PxMaterial* mMaterial = nullptr;
	PxRigidActor* mActor = nullptr;
	PhxBodyStateType mBodyState = PhxBodyStateType::eDynamic;
};