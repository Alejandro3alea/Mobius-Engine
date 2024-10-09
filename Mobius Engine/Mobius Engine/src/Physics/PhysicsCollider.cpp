#include "PhysicsCollider.h"
#include "Editor/ImGuiWidgets.h"

SphereCollider::SphereCollider(const float Radius) 
{
	mGeometry = new PxSphereGeometry(Radius);
}

void SphereCollider::GenerateGeometry()
{
	float Size = 1.0f;

	if (mGeometry)
	{
		Size = mGeometry->radius;
		delete mGeometry;
	}

	mGeometry = new PxSphereGeometry(Size);
}

bool SphereCollider::OnGui()
{
	return ImGui::OnGui("Radius", mGeometry->radius);
}

void SphereCollider::FromJson(json& Val)
{
	Val["Radius"] >> mGeometry->radius;
}

json& SphereCollider::ToJson(json& Val)
{
	Val["Radius"] << mGeometry->radius;
	return Val;
}

BoxCollider::BoxCollider(glm::vec3 Size)
{
	mGeometry = new PxBoxGeometry(Size.x, Size.y, Size.z);
}

void BoxCollider::GenerateGeometry()
{
	PxVec3 Size(1.0f);

	if (mGeometry)
	{
		Size = mGeometry->halfExtents;
		delete mGeometry;
	}

	mGeometry = new PxBoxGeometry(Size);
}

bool BoxCollider::OnGui()
{
	bool changed = ImGui::OnGui("Half Extents", mGeometry->halfExtents);

	if (changed)
		for (unsigned i = 0; i < 3; i++)
			if (mGeometry->halfExtents[i] <= 0.0f)
				mGeometry->halfExtents[i] = 0.01f;

	return changed;
}

void BoxCollider::FromJson(json& Val)
{
	Val["Half Extents"] >> mGeometry->halfExtents;
}

json& BoxCollider::ToJson(json& Val)
{
	Val["Half Extents"] << mGeometry->halfExtents;
	return Val;
}
