#include "Factory.h"
#include "Components/Physics/RigidBody.h"
#include "Components/Graphics/LightComp.h"
#include "Components/Graphics/ModelRenderer.h"
#include "Components/Audio/AudioEmitter.h"
#include "Components/Audio/AudioListener.h"

FactoryManager* FactoryManager::mpInstance;

void FactoryManager::Initialize()
{
	// ----------------- COMPONENTS ----------------- 
	RegisterCreator<RigidBody>();
	RegisterCreator<ModelRenderer>();
	RegisterCreator<MeshRenderer>();
	RegisterCreator<LightComp>();
	RegisterCreator<AudioEmitter>();
	RegisterCreator<AudioListener>();
}
