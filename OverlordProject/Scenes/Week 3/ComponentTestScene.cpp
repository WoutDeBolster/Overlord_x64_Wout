#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/SpherePrefab.h"

ComponentTestScene::ComponentTestScene()
	: GameScene(L"ComponentTestScene")
{
}

void ComponentTestScene::Initialize()
{
	//m_SceneContext.settings.enableOnGUI = true;

	auto pBouncyMat = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.9f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMat);

	// sphere 1 (group 1 and group 2)
	auto pSphereObj = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4(Colors::Red)));
	auto pSphereActor = pSphereObj->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMat);
	pSphereActor->SetCollisionGroup(CollisionGroup::Group1);

	pSphereActor->GetTransform()->Translate(0.f, 40.f, 0.f);

	// sphere 2 (group 0) + (ignore group 1)
	pSphereObj = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4(Colors::Green)));
	pSphereActor = pSphereObj->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMat);
	pSphereActor->SetCollisionIgnoreGroups(CollisionGroup::Group1);

	pSphereActor->GetTransform()->Translate(0.f, 30.f, 0.f);

	// sphere 3 (group 0)
	pSphereObj = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4(Colors::Blue)));
	pSphereActor = pSphereObj->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMat);

	pSphereActor->GetTransform()->Translate(0.f, 20.f, 0.f);
}
