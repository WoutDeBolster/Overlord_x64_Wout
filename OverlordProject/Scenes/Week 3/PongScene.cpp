#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/SpherePrefab.h"
#include "Prefabs/CubePrefab.h"

PongScene::PongScene()
	: GameScene(L"PongScene")
{
}

void PongScene::Initialize()
{
	// initializing random seed
	srand(static_cast<unsigned int>(time(NULL)));

	m_SceneContext.settings.enableOnGUI = true;

	auto pBouncyMat = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);
	auto pNormalMat = PxGetPhysics().createMaterial(0.f, 0.f, 0.f);

	GameSceneExt::CreatePhysXGroundPlane(*this, pNormalMat);

	// cam top down
	auto camEmpty = new GameObject();
	auto cam = new CameraComponent();
	camEmpty->GetTransform()->Translate(0.f, 50.f, 0.f);
	camEmpty->GetTransform()->Rotate(90.f, 0.f, 0.f);
	camEmpty->AddComponent(cam);
	AddChild(camEmpty);
	SetActiveCamera(cam);


	// ball
	m_pBall = new GameObject();
	m_pBall->AddChild<SpherePrefab>(new SpherePrefab(1.f, 10, XMFLOAT4(Colors::Red)));
	auto pSphereActor = m_pBall->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMat);

	m_pBall->GetTransform()->Translate(0.f, 5.f, 0.f);

	// paddels
	XMFLOAT3 paddleDim{ 1.f, 1.f, 10.f };
	m_pCubeLeft = new GameObject();
	m_pCubeLeft = AddChild(new CubePrefab(paddleDim, XMFLOAT4(Colors::Gray)));
	auto pCubeActorLeft = m_pCubeLeft->AddComponent(new RigidBodyComponent(false));
	pCubeActorLeft->AddCollider(PxBoxGeometry{ paddleDim.x / 2.f, paddleDim.y / 2.f, paddleDim.z / 2.f }, *pNormalMat);
	pCubeActorLeft->SetConstraint(RigidBodyConstraint::TransX, true);
	pCubeActorLeft->SetConstraint(RigidBodyConstraint::TransZ, true);
	pCubeActorLeft->SetConstraint(RigidBodyConstraint::AllRot, true);

	m_pCubeLeft->GetTransform()->Translate(-30.f, 1.f, 0.f);


	m_pCubeRight = new GameObject();
	m_pCubeRight = AddChild(new CubePrefab(paddleDim, XMFLOAT4(Colors::Gray)));
	auto pCubeActorRight = m_pCubeRight->AddComponent(new RigidBodyComponent(false));
	pCubeActorRight->AddCollider(PxBoxGeometry{ paddleDim.x / 2.f, paddleDim.y / 2.f, paddleDim.z / 2.f }, *pNormalMat);
	pCubeActorRight->SetConstraint(RigidBodyConstraint::TransX, true);
	pCubeActorRight->SetConstraint(RigidBodyConstraint::TransZ, true);
	pCubeActorRight->SetConstraint(RigidBodyConstraint::AllRot, true);

	m_pCubeRight->GetTransform()->Translate(30.f, 1.f, 0.f);

	// input
	const auto inputAction1 = InputAction(InputActions::RightPeddle_Up, InputState::down, VK_UP);
	m_SceneContext.pInput->AddInputAction(inputAction1);

	const auto inputAction2 = InputAction(InputActions::RightPeddle_Down, InputState::down, VK_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction2);

	const auto inputAction3 = InputAction(InputActions::LeftPeddle_Up, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction3);

	const auto inputAction4 = InputAction(InputActions::LeftPeddle_Down, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction4);

	const auto inputAction5 = InputAction(InputActions::LaunchBall, InputState::released, VK_SPACE);
	m_SceneContext.pInput->AddInputAction(inputAction5);
}

void PongScene::Update()
{
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::RightPeddle_Up))
	{
		m_pCubeRight->GetComponent<RigidBodyComponent>()->AddForce({ 0.f, 0.f, 5.f }, physx::PxForceMode::eFORCE);
	}
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::RightPeddle_Down))
	{
		m_pCubeRight->GetComponent<RigidBodyComponent>()->AddForce({ 0.f, 0.f, -5.f }, physx::PxForceMode::eFORCE);
	}

	if (m_SceneContext.pInput->IsActionTriggered(InputActions::LeftPeddle_Up))
	{
		m_pCubeLeft->GetComponent<RigidBodyComponent>()->AddForce({ 0.f, 0.f, 5.f }, physx::PxForceMode::eFORCE);
	}
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::LeftPeddle_Down))
	{
		m_pCubeLeft->GetComponent<RigidBodyComponent>()->AddForce({ 0.f, 0.f, -5.f }, physx::PxForceMode::eFORCE);
	}

	if (m_SceneContext.pInput->IsActionTriggered(InputActions::LaunchBall))
	{
		DirectX::XMFLOAT3 rad = { float(rand() % 20) - 10, 0.f, 0.f };
		m_pBall->GetComponent<RigidBodyComponent>()->AddForce({ rad }, physx::PxForceMode::eIMPULSE);
	}
}
