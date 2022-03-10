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
	m_SceneContext.settings.clearColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_SceneContext.settings.drawGrid = false;

	// initializing random seed
	srand(static_cast<unsigned int>(time(NULL)));

	m_SceneContext.settings.enableOnGUI = true;

	auto& physX = PxGetPhysics();

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
	std::cout << "Press space to launche" << std::endl;
	m_pBall = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4(Colors::Red)));
	auto pSphereActor = m_pBall->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMat);
	pSphereActor->SetConstraint(RigidBodyConstraint::TransZ, true);
	pSphereActor->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	m_pBall->GetTransform()->Translate(0.f, 5.f, 0.f);

	// paddels
	XMFLOAT3 paddleDim{ 10.f, 1.f, 1.f };
	m_pCubeLeft = new GameObject();
	m_pCubeLeft = AddChild(new CubePrefab(paddleDim, XMFLOAT4(Colors::Gray)));
	auto pCubeActorLeft = m_pCubeLeft->AddComponent(new RigidBodyComponent(false));
	pCubeActorLeft->GetTransform()->Rotate(90.f, 90.f, 0.f);
	pCubeActorLeft->AddCollider(PxCapsuleGeometry{ 1.f, 5.f }, *pBouncyMat);
	pCubeActorLeft->SetConstraint(RigidBodyConstraint::TransX, true);
	pCubeActorLeft->SetConstraint(RigidBodyConstraint::TransZ, true);
	pCubeActorLeft->SetConstraint(RigidBodyConstraint::AllRot, true);
	pCubeActorLeft->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	pCubeActorLeft->SetKinematic(true);

	m_pCubeLeft->GetTransform()->Translate(-30.f, 5.f, 0.f);


	m_pCubeRight = new GameObject();
	m_pCubeRight = AddChild(new CubePrefab(paddleDim, XMFLOAT4(Colors::Gray)));
	auto pCubeActorRight = m_pCubeRight->AddComponent(new RigidBodyComponent(false));
	pCubeActorRight->GetTransform()->Rotate(90.f, 90.f, 0.f);
	pCubeActorRight->AddCollider(PxCapsuleGeometry{ 1.f, 5.f }, *pBouncyMat);
	pCubeActorRight->SetConstraint(RigidBodyConstraint::TransX, true);
	pCubeActorRight->SetConstraint(RigidBodyConstraint::TransZ, true);
	pCubeActorRight->SetConstraint(RigidBodyConstraint::AllRot, true);
	pCubeActorRight->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	pCubeActorRight->SetKinematic(true);

	m_pCubeRight->GetTransform()->Translate(30.f, 5.f, 0.f);

	// horizontal walls
	auto pWallActor = physX.createRigidStatic(PxTransform(0.f, 0.f, 20.f));
	auto pWallShape = PxRigidActorExt::createExclusiveShape(*pWallActor, PxBoxGeometry(100.f, 10.f, 1.f), *pBouncyMat);
	GetPhysxProxy()->AddActor(*pWallActor);

	pWallActor = physX.createRigidStatic(PxTransform(0.f, 0.f, -20.f));
	pWallShape = PxRigidActorExt::createExclusiveShape(*pWallActor, PxBoxGeometry(100.f, 10.f, 1.f), *pBouncyMat);
	GetPhysxProxy()->AddActor(*pWallActor);

	// triggers
	m_pWallLeft = AddChild(new GameObject());
	auto wallTriggerActor = m_pWallLeft->AddComponent(new RigidBodyComponent(true));
	wallTriggerActor->AddCollider(PxBoxGeometry{ 1.f, 10.f, 20.f }, *pBouncyMat, true);
	m_pWallLeft->GetTransform()->Translate(-38.f, 5.f, 0.f);
	m_pWallLeft->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				std::cout << "leftTriggerHit" << std::endl;
				Reset();
			}
		}
	);

	m_pWallRight = AddChild(new GameObject());
	wallTriggerActor = m_pWallRight->AddComponent(new RigidBodyComponent(true));
	wallTriggerActor->AddCollider(PxBoxGeometry{ 1.f, 10.f, 20.f }, *pBouncyMat, true);
	m_pWallRight->GetTransform()->Translate(38.f, 5.f, 0.f);
	m_pWallRight->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				std::cout << "RightTriggerHit" << std::endl;
				Reset();
			}
		}
	);

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

	const auto inputAction6 = InputAction(InputActions::ResetButton, InputState::released, 'R');
	m_SceneContext.pInput->AddInputAction(inputAction6);
}

inline XMFLOAT3 AddVector(XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

void PongScene::Update()
{
	XMFLOAT3 movement = XMFLOAT3{ 0.f, 0.f, m_MovementSpeed * GetSceneContext().pGameTime->GetElapsed() };
	XMFLOAT3 reversedMovement = movement;
	reversedMovement.z = -reversedMovement.z;

	XMFLOAT3 leftPeddlePosUp = AddVector(m_pCubeLeft->GetTransform()->GetPosition(), movement);
	XMFLOAT3 leftPeddlePosDown = AddVector(m_pCubeLeft->GetTransform()->GetPosition(), reversedMovement);

	XMFLOAT3 rightPeddlePosUp = AddVector(m_pCubeRight->GetTransform()->GetPosition(), movement);
	XMFLOAT3 rightPeddlePosDown = AddVector(m_pCubeRight->GetTransform()->GetPosition(), reversedMovement);

	if (m_SceneContext.pInput->IsActionTriggered(InputActions::RightPeddle_Up) && rightPeddlePosUp.z < m_ZMax)
	{
		m_pCubeRight->GetTransform()->Translate(rightPeddlePosUp);
	}
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::RightPeddle_Down) && rightPeddlePosDown.z > m_Zmin)
	{
		m_pCubeRight->GetTransform()->Translate(rightPeddlePosDown);
	}

	if (m_SceneContext.pInput->IsActionTriggered(InputActions::LeftPeddle_Up) && leftPeddlePosUp.z < m_ZMax)
	{
		m_pCubeLeft->GetTransform()->Translate(leftPeddlePosUp);
	}
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::LeftPeddle_Down) && leftPeddlePosDown.z > m_Zmin)
	{
		m_pCubeLeft->GetTransform()->Translate(leftPeddlePosDown);
	}

	if (m_SceneContext.pInput->IsActionTriggered(InputActions::LaunchBall))
	{
		DirectX::XMFLOAT3 rad = { 20.f, 0.f, float(rand() % 20) - 10 };
		m_pBall->GetComponent<RigidBodyComponent>()->AddForce({ rad }, physx::PxForceMode::eIMPULSE);
	}

	if (m_SceneContext.pInput->IsActionTriggered(InputActions::ResetButton))
	{
		Reset();
	}
}

void PongScene::Reset()
{
	m_pBall->GetTransform()->Translate(0.f, 5.f, 0.f);
	m_pCubeLeft->GetTransform()->Translate(-30.f, 5.f, 0.f);
	m_pCubeRight->GetTransform()->Translate(30.f, 5.f, 0.f);
}
