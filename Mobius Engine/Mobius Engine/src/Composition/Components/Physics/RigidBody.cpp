#include "RigidBody.h"
#include "Editor/Editor.h"
#include "Editor/ImGuiWidgets.h"

RigidBody::RigidBody()
{
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

RigidBody::~RigidBody()
{
	delete mCollider;
	mMaterial->release();
	PhysicsMgr->RemoveRigidBody(this);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void RigidBody::Initialize()
{
	mMaterial = PhysicsMgr->SDK()->createMaterial(0.7f, 0.7f, 0.3f);
	ChangeCollider(PhxColliderType::eBoxCollider);
	PhysicsMgr->AddRigidBody(this);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

// @TODO: WORLD TRANSFORM
void RigidBody::BeginUpdate()
{
	const glm::vec3& pos = mOwner->transform.pos;
	const glm::quat& rot = mOwner->transform.rotation;

	PxQuat q(rot.x, rot.y, rot.z, rot.w);
	PxTransform Transform(pos.x, pos.y, pos.z, q);

	mActor->setGlobalPose(Transform);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

// @TODO: WORLD TRANSFORM
void RigidBody::EndUpdate()
{
	PxTransform Transform = mActor->getGlobalPose();

	for (unsigned i = 0; i < 3; i++)
		mOwner->transform.pos[i] = Transform.p[i];

	const PxQuat& rot = Transform.q;
	mOwner->transform.rotation = glm::quat(rot.w, rot.x, rot.y, rot.z);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void RigidBody::OnGui()
{
	if (ImGui::BeginCompOnGui("RigidBody", Editor->mIcons["RigidBody"]))
	{
		ImGui::OnGuiHeader("Collider Type");
		std::string colliderTypes[] = { "Box Collider", "Sphere Collider" };
		const unsigned colliderIdx = static_cast<unsigned>(mColliderType);
		std::string comboLabel = colliderTypes[colliderIdx];
		if (ImGui::BeginCombo("##Collider Type", comboLabel.c_str()))
		{
			for (int n = 0; n < IM_ARRAYSIZE(colliderTypes); n++)
			{
				const char* currItem = colliderTypes[n].c_str();
				const bool isSelected = (colliderIdx == n);
				if (ImGui::Selectable(currItem, isSelected))
					ChangeCollider(static_cast<PhxColliderType>(n));

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::NextColumn();

		ImGui::OnGuiHeader("Body State Type");
		if (ImGui::RadioButton("Dynamic", mBodyState == PhxBodyStateType::eDynamic))
		{
			mBodyState = PhxBodyStateType::eDynamic;
			ChangeState(); 
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Static", mBodyState == PhxBodyStateType::eStatic)) 
		{ 
			mBodyState = PhxBodyStateType::eStatic;
			ChangeState();
		}
		ImGui::NextColumn();

		if (mCollider && mCollider->OnGui())
			ChangeState();
	}
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void RigidBody::FromJson(json& Val)
{
	int colliderTypeVal, bodyStateTypeVal;
	Val["Collider Type"] >> colliderTypeVal;
	mColliderType = static_cast<PhxColliderType>(colliderTypeVal);
	Val["Body State"] >> bodyStateTypeVal;
	mBodyState = static_cast<PhxBodyStateType>(bodyStateTypeVal);
	ChangeCollider(mColliderType);
	mCollider->FromJson(Val["Collider"]);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

json& RigidBody::ToJson(json& Val)
{
	Val["Collider Type"] << static_cast<int>(mColliderType);
	Val["Body State"] << static_cast<int>(mBodyState);
	mCollider->ToJson(Val["Collider"]);
	return Val;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void RigidBody::ChangeCollider(const PhxColliderType ColliderType)
{
	mColliderType = ColliderType;

	if (mCollider)
		delete mCollider;

	switch (ColliderType)
	{
	case PhxColliderType::eBoxCollider:
		mCollider = new BoxCollider(mOwner->worldTransform.scale);
		break;
	case PhxColliderType::eSphereCollider:
		mCollider = new SphereCollider(mOwner->worldTransform.scale.x);
		break;
	}

	ChangeState();
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void RigidBody::ChangeState()
{
	if (mActor)
	{
		PhysicsMgr->GetScene()->removeActor(*mActor);
		mActor->release();
	}

	// @TODO: This might cause problems in the future
	const glm::vec3& pos = mOwner->transform.pos;
	const glm::quat& rot = mOwner->transform.rotation;

	PxQuat q(rot.x, rot.y, rot.z, rot.w);
	PxTransform Transform(pos.x, pos.y, pos.z, q);

	PxRigidDynamic* DynamicActor;
	PxRigidStatic* StaticActor;
	switch (mBodyState)
	{
	case PhxBodyStateType::eDynamic:
		DynamicActor = PxCreateDynamic(*PhysicsMgr->SDK(), Transform, *mCollider->GetGeometry(), *mMaterial, 1.0f);
		DynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true); // Activar detección continua de colisiones
		mActor = DynamicActor;
		break;
	case PhxBodyStateType::eStatic:
		StaticActor = PxCreateStatic(*PhysicsMgr->SDK(), Transform, *mCollider->GetGeometry(), *mMaterial);
		mActor = StaticActor;
		break;
	};
	
	//mShape = PxRigidActorExt::createExclusiveShape(*mActor, *mCollider->GetGeometry(), *defaultMaterial);

	PhysicsMgr->GetScene()->addActor(*mActor);

	/**mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 1, 0, 50), *mMaterial);
	mScene->addActor(*groundPlane);

	float halfExtent = .5f;
	physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
	physx::PxU32 size = 30;
	physx::PxTransform t(physx::PxVec3(0));
	for (physx::PxU32 i = 0; i < size; i++) {
		for (physx::PxU32 j = 0; j < size - i; j++) {
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
			physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			mScene->addActor(*body);
		}
	}
	shape->release();*/

}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void RigidBody::UpdatePhysicsData()
{
	/*switch (mBodyState)
	{
	case PhxBodyStateType::eDynamic:
		PxRigidDynamic* DynamicActor = PxCreateDynamic(*PhysicsMgr->SDK(), sphereTransform, sphereGeometry, *defaultMaterial, 1.0f);
		DynamicActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true); // Activar detección continua de colisiones
		mActor = DynamicActor;
		break;
	case PhxBodyStateType::eStatic:
		PxRigidStatic* StaticActor = PxCreateStatic(*PhysicsMgr->SDK(), sphereTransform, sphereGeometry, *defaultMaterial);
		mActor = StaticActor;
		break;
	}*/
}
