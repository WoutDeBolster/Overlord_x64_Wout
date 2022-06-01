#include "stdafx.h"
#include "Portal.h"

#include "Prefabs/Character.h"
#include "Prefabs/CubePrefab.h"
#include "Materials/ColorMaterial.h"


Portal::Portal()
	: GameScene(L"PortalScene")
{
}

void Portal::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	// portals

	//m_pBlueCam = new CameraComponent();
	//m_pOrangeCam = new CameraComponent();

	InitCharater();
	InitObjects();
	InitPortals();

	//// cam top down
	//auto camEmpty = new GameObject();
	//auto cam = new CameraComponent();
	//camEmpty->GetTransform()->Translate(0.f, 5.f, -4.f);
	//camEmpty->GetTransform()->Rotate(0.f, 0.f, 0.f);
	//camEmpty->AddComponent(cam);
	//AddChild(camEmpty);
	//SetActiveCamera(cam);
}

void Portal::Update()
{
	if (InputManager::IsMouseButton(InputState::down, VK_MBUTTON))
	{
		PxVec3 hitPos{};

		if (const auto pPickedObject = m_SceneContext.pCamera->Pick(CollisionGroup::Group5, hitPos))
		{
			if (pPickedObject->GetTag() == L"moveable")
			{

				XMFLOAT3 objectPos{ pPickedObject->GetTransform()->GetPosition() };
				XMFLOAT3 characterPos{ m_pCharacter->GetTransform()->GetPosition() };
				XMFLOAT3 characterForward{ m_pCharacter->GetTransform()->GetForward() };
				XMVECTOR objectCharVec{ XMLoadFloat3(&objectPos) - XMLoadFloat3(&characterPos) };
				XMFLOAT3 distanceObject{};
				XMStoreFloat3(&distanceObject, XMVector3Length(objectCharVec));
				XMFLOAT2 mousePos{ static_cast<float>(InputManager::GetMousePosition().x),
									static_cast<float>(InputManager::GetMousePosition().y) };

				float halfViewWidth{ m_SceneContext.windowWidth / 2.f };
				float halfViewHeight{ m_SceneContext.windowHeight / 2.f };

				XMFLOAT2 mousPosNDC{};
				mousPosNDC.x = (mousePos.x - halfViewWidth) / halfViewWidth;
				mousPosNDC.y = (halfViewHeight - mousePos.y) / halfViewHeight;

				pPickedObject->GetTransform()->Translate(characterPos.x + (characterForward.x * distanceObject.x),
					characterPos.y + (characterForward.y * distanceObject.y),
					characterPos.z + (characterForward.z * distanceObject.z));
			}
		}
	}

	// set portals
	if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
	{
		PxVec3 hitPos{};

		if (const auto pPickedObject = m_SceneContext.pCamera->Pick(CollisionGroup::Group2, hitPos))
		{
			if (pPickedObject->GetTag() == L"PortalSurface")
			{
				m_pBluePortal->GetTransform()->Translate(hitPos.x, hitPos.y, hitPos.z);
			}
		}
	}

	if (InputManager::IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		PxVec3 hitPos{};

		if (const auto pPickedObject = m_SceneContext.pCamera->Pick(CollisionGroup::Group2, hitPos))
		{
			if (pPickedObject->GetTag() == L"PortalSurface")
			{
				m_pOrangePortal->GetTransform()->Translate(hitPos.x, hitPos.y, hitPos.z);
				//pPickedObject->GetTransform()->GetForward()
			}
		}
	}

	// hitting portal
}

void Portal::InitPortals()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	m_pBluePortal = AddChild(new GameObject());
	m_pBluePortal->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Portal/PortalPlaneTest.ovm"));
	m_pBluePortalMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pBluePortalMat->SetDiffuseTexture(L"Textures/Portal/BluePortal.png");
	m_pBluePortal->GetComponent<ModelComponent>()->SetMaterial(m_pBluePortalMat);

	m_pBluePortal->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pBluePortal->GetTransform()->Scale(0.02f, 0.02f, 0.02f);

	m_pBluePortal->SetOnTriggerCallBack([=](GameObject*, GameObject* person, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER && person->GetTag() == L"Character")
			{
				m_pCharacter->GetTransform()->Translate(m_pOrangePortal->GetTransform()->GetPosition());

				std::cout << "odsjakhdaswlk";
			}
		}
	);

	m_pOrangePortal = AddChild(new GameObject());
	m_pOrangePortal->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Portal/PortalPlaneTest.ovm"));
	m_pOrangePortalMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pOrangePortalMat->SetDiffuseTexture(L"Textures/Portal/OrangePortal.png");
	m_pOrangePortal->GetComponent<ModelComponent>()->SetMaterial(m_pOrangePortalMat);
	const auto pOrangePortalActor = m_pOrangePortal->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Portal/PortalPlaneTest.ovpt");
	pOrangePortalActor->AddCollider(PxTriangleMeshGeometry{ pPxTriangleMesh, PxMeshScale({1.f, 1.f, 1.f}) }, *pDefaultMaterial);

	m_pOrangePortal->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pOrangePortal->GetTransform()->Scale(0.02f, 0.02f, 0.02f);

	m_pOrangePortal->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				m_pCharacter->GetTransform()->Translate(m_pBluePortal->GetTransform()->GetPosition());
				std::cout << "odsjakhdaswlk";
			}
		}
	);
}

void Portal::InitCharater()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Portal/Chell.ovm"));
	m_pCharacterMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pCharacterMat->SetDiffuseTexture(L"Textures/Portal/chell_body.png");
	m_pCharacter->GetComponent<ModelComponent>()->SetMaterial(m_pCharacterMat);
	m_pCharacter->SetTag(L"Character");

	const auto pCharacterControlerComp = m_pCharacter->GetComponent<ControllerComponent>();
	pCharacterControlerComp->SetCollisionGroup(CollisionGroup::Group5 & CollisionGroup::Group2);

	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);

	if (const auto pAnimator = m_pCharacter->GetComponent<ModelComponent>()->GetAnimator())
	{
		pAnimator->SetAnimation(2);
		pAnimator->Play();
	}

	// gun
	m_pPortalGun = new GameObject();
	m_pCharacter->AddChild(m_pPortalGun);
	m_pPortalGun->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Portal/portal-gun.ovm"));
	m_pPortalGunMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pPortalGunMat->SetDiffuseTexture(L"Textures/Portal/v_portalgun.png");
	m_pPortalGun->GetComponent<ModelComponent>()->SetMaterial(m_pPortalGunMat);

	m_pPortalGun->GetTransform()->Translate(.5f, .5f, 1.7f);
	m_pPortalGun->GetTransform()->Scale(0.15f, 0.15f, 0.15f);
	m_pPortalGun->GetTransform()->Rotate(-190.f, -10.f, 0.f, true);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	pLevelObject->SetTag(L"PortalSurface");
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/SimpleLevel.ovm"));
	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/SimpleLevel.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .5f,.5f,.5f })), *pDefaultMaterial);
	pLevelObject->GetTransform()->Scale(.5f, .5f, .5f);
	pLevelActor->SetCollisionGroup(CollisionGroup::Group5);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void Portal::InitObjects()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	m_pCube = AddChild(new GameObject());
	m_pCube->SetTag(L"moveable");
	m_pCube->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Portal/weightedcube.ovm"));

	const auto pCubeActor = m_pCube->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Portal/weightedcube.ovpt");
	pCubeActor->AddCollider(PxTriangleMeshGeometry{ pPxTriangleMesh, PxMeshScale({.05f, .05f, .05f}) }, *pDefaultMaterial);
	//pCubeActor->AddCollider(PxCapsuleGeometry{ 1.f, m_pCube->GetTransform()->GetWorldScale().y / 2.f }, *pDefaultMaterial);
	//const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Portal/weightedcube.ovpc");
	//pCubeActor->AddCollider(PxConvexMeshGeometry{ pConvexMesh,  PxMeshScale({.05f, .05f, .05f}) }, *pDefaultMaterial, false);
	//pCubeActor->SetConstraint(RigidBodyConstraint::TransY, true);
	//pCubeActor->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	pCubeActor->SetCollisionGroup(CollisionGroup::Group2);
	pCubeActor->GetTransform()->Scale(.05f, .05f, .05f);
	pCubeActor->GetTransform()->Translate(0.f, 1.f, 10.f);

	m_pCubeMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pCubeMat->SetDiffuseTexture(L"Textures/Portal/metal_box.png");
	m_pCube->GetComponent<ModelComponent>()->SetMaterial(m_pCubeMat);
}
