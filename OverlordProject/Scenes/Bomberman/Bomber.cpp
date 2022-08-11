#include "stdafx.h"
#include "Bomber.h"

#include "Prefabs/Character.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

Bomber::Bomber()
	: GameScene(L"BomberScene")
{
}

void Bomber::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	// cam top down
	auto camEmpty = new GameObject();
	auto cam = new CameraComponent();
	camEmpty->GetTransform()->Translate(15.f, 80.f, -20.f);
	camEmpty->GetTransform()->Rotate(65.f, 0.f, 0.f);
	camEmpty->AddComponent(cam);
	AddChild(camEmpty);
	SetActiveCamera(cam);

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	InitCharacter();
	InitLevel();

	//// sphere 1 (group 1 and group 2)
	//auto pSphereObj = new SpherePrefab(1.f, 10, XMFLOAT4(Colors::Red));
	//auto pSphereActor = pSphereObj->AddComponent(new RigidBodyComponent(false));
	//pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pDefaultMaterial);
	//pSphereActor->SetCollisionGroup(CollisionGroup::Group1);

	//pSphereActor->GetTransform()->Translate(0.f, 40.f, 0.f);
	//pSphereActor->GetTransform()->Scale(5.f);
	//AddChild(pSphereObj);
}

void Bomber::Update()
{
	const float elapsed{ m_SceneContext.pGameTime->GetElapsed() };

	if (m_SceneContext.pInput->IsActionTriggered(CharacterPlaceBomb))
	{
		SpawnBomb();
	}

	for (size_t i = 0; i < m_Bombs.size(); i++)
	{
		m_Bombs[i].countDown -= elapsed;
		if (m_Bombs[i].countDown <= 0.f)
		{
			SpawnParticles(m_Bombs[i]);
			RemoveChild(m_Bombs[i].object, true);
			m_Bombs.erase(m_Bombs.begin() + i);
		}
	}
}

void Bomber::InitCharacter()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial, 0.5f, 0.1f };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterPlaceBomb;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);
	const auto pModel = new ModelComponent(L"BomberMan/Ch09_nonPBR/Ch09_nonPBR2.ovm");
	m_pCharacter->AddComponent<ModelComponent>(pModel);

	if (const auto pAnimator = pModel->GetAnimator())
	{
		pAnimator->SetAnimation(2);
		pAnimator->Play();
	}

	DiffuseMaterial* pCharaterMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pCharaterMat->SetDiffuseTexture(L"BomberMan/Ch09_nonPBR/Ch09_1001_Diffuse.png");
	m_pCharacter->GetComponent<ModelComponent>()->SetMaterial(pCharaterMat);
	m_pCharacter->GetTransform()->Scale(0.05f, 0.05f, 0.05f);

	m_pCharacter->GetComponent<ControllerComponent>()->SetCollisionGroup(CollisionGroup::Group1 & CollisionGroup::Group0);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterPlaceBomb, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void Bomber::InitLevel()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	const auto pGroundMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial1->SetDiffuseTexture(L"Textures/ironGreen.jpg");
	const auto pGroundMaterial2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial2->SetDiffuseTexture(L"Textures/ironGrey.jpg");
	const auto pWallMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pWallMaterial1->SetDiffuseTexture(L"Textures/brickGrey.jpg");

	////Simple Level
	//const auto pLevelObject = AddChild(new GameObject());
	//const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/SimpleLevel.ovm"));
	//pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	//const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	//const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/SimpleLevel.ovpt");
	//pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .5f,.5f,.5f })), *pDefaultMaterial);
	//pLevelObject->GetTransform()->Scale(.5f, .5f, .5f);

	// ***********
	// BOTTOM GRID
	// ***********
	GameObject* pLevelGrid{ new GameObject{} };
	bool ColorGreen{ true };
	XMFLOAT3 dimensions{ 1.f, 1.f, 1.f };
	XMFLOAT4 forestGreen{ 34.f / 255.f ,139.f / 255.f,34.f / 255.f, 1.f };
	XMFLOAT4 lightGray{ 211.f / 255.f ,211.f / 255.f ,211.f / 255.f, 1.f };
	const float scale{ 5.f };
	for (int y = 0; y < m_GridHeight; y++)
	{
		for (int x = 0; x < m_GridWidth; x++)
		{
			if (ColorGreen)
			{
				const auto pGroundObj = new GameObject();

				const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
				pGroundModel->SetMaterial(pGroundMaterial1);
				pGroundObj->AddComponent(pGroundModel);

				const auto pColisionObj = new GameObject();
				auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
				pCubeActor->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
				pCubeActor->SetCollisionGroup(CollisionGroup::Group1);

				pColisionObj->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale));
				pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale) + 2.5f);
				pGroundObj->GetTransform()->Scale(scale);
				pLevelGrid->AddChild(pGroundObj);
				pLevelGrid->AddChild(pColisionObj);
			}
			else
			{
				const auto pGroundObj = new GameObject();

				const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
				pGroundModel->SetMaterial(pGroundMaterial2);
				pGroundObj->AddComponent(pGroundModel);

				const auto pColisionObj = new GameObject();
				auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
				pCubeActor->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
				pCubeActor->SetCollisionGroup(CollisionGroup::Group1);

				pColisionObj->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale));
				pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale) + 2.5f);
				pGroundObj->GetTransform()->Scale(scale);
				pLevelGrid->AddChild(pGroundObj);
				pLevelGrid->AddChild(pColisionObj);
			}
			ColorGreen = !ColorGreen;
		}
		ColorGreen = !ColorGreen;
	}
	pLevelGrid->GetTransform()->Translate(0.f, 0.f, 0.f);
	AddChild(pLevelGrid);

	// ******
	// BORDER
	// ******
	GameObject* pLevelBorder{ new GameObject{} };
	XMFLOAT4 darkGrey{ 169.f / 255.f ,169.f / 255.f,169.f / 255.f, 1.f };
	int borderWidth{ m_GridWidth + 2 }; // +2 for the corners
	int borderHeight{ m_GridHeight }; // only one needs +2
	for (int x = 0; x < borderWidth; x++)
	{
		// bottom row
		const auto pWallObjBottom = new GameObject();

		const auto pWallModelBottom = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelBottom->SetMaterial(pWallMaterial1);
		pWallObjBottom->AddComponent(pWallModelBottom);

		const auto pColisionObjBottom = new GameObject();
		auto pCubeActorBottom = pColisionObjBottom->AddComponent(new RigidBodyComponent(true));
		pCubeActorBottom->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
		pCubeActorBottom->SetCollisionGroup(CollisionGroup::Group1);

		pWallObjBottom->GetTransform()->Translate(static_cast<float>(x) - 1.f, 1.f, -dimensions.z + 0.5f);
		pColisionObjBottom->GetTransform()->Translate(static_cast<float>(x * scale) - scale, scale, -dimensions.z * scale);
		pLevelBorder->AddChild(pWallObjBottom);
		pLevelBorder->AddChild(pColisionObjBottom);

		// top row
		const auto pWallObjTop = new GameObject();

		const auto pWallModelTop = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelTop->SetMaterial(pWallMaterial1);
		pWallObjTop->AddComponent(pWallModelTop);

		const auto pColisionObjTop = new GameObject();
		auto pCubeActorTop = pColisionObjTop->AddComponent(new RigidBodyComponent(true));
		pCubeActorTop->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
		pCubeActorTop->SetCollisionGroup(CollisionGroup::Group1);

		pWallObjTop->GetTransform()->Translate(static_cast<float>(x) - 1.f, 1.f, dimensions.z * m_GridHeight + 0.5f);
		pColisionObjTop->GetTransform()->Translate(static_cast<float>(x * scale) - scale, scale, +dimensions.z * m_GridHeight * scale);
		pLevelBorder->AddChild(pWallObjTop);
		pLevelBorder->AddChild(pColisionObjTop);
	}
	for (int y = 0; y < borderHeight; y++)
	{
		// bottom row
		const auto pWallObjBottom = new GameObject();

		const auto pWallModelBottom = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelBottom->SetMaterial(pWallMaterial1);
		pWallObjBottom->AddComponent(pWallModelBottom);

		const auto pColisionObjBottom = new GameObject();
		auto pCubeActorBottom = pColisionObjBottom->AddComponent(new RigidBodyComponent(true));
		pCubeActorBottom->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
		pCubeActorBottom->SetCollisionGroup(CollisionGroup::Group1);

		pWallObjBottom->GetTransform()->Translate(-dimensions.x, 1.f, static_cast<float>(y) + 0.5f);
		pColisionObjBottom->GetTransform()->Translate(-dimensions.x * scale, scale, static_cast<float>(y * scale));
		pLevelBorder->AddChild(pWallObjBottom);
		pLevelBorder->AddChild(pColisionObjBottom);

		// top row
		const auto pWallObjTop = new GameObject();

		const auto pWallModelTop = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelTop->SetMaterial(pWallMaterial1);
		pWallObjTop->AddComponent(pWallModelTop);

		const auto pColisionObjTop = new GameObject();
		auto pCubeActorTop = pColisionObjTop->AddComponent(new RigidBodyComponent(true));
		pCubeActorTop->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
		pCubeActorTop->SetCollisionGroup(CollisionGroup::Group1);

		pWallObjTop->GetTransform()->Translate(dimensions.x * m_GridWidth, 1.f, static_cast<float>(y) + 0.5f);
		pColisionObjTop->GetTransform()->Translate(dimensions.x * m_GridWidth * scale, scale, static_cast<float>(y * scale));
		pLevelBorder->AddChild(pWallObjTop);
		pLevelBorder->AddChild(pColisionObjTop);
	}
	pLevelBorder->GetTransform()->Translate(0.f, 0.f, 0.f);
	pLevelBorder->GetTransform()->Scale(scale);
	AddChild(pLevelBorder);

	// ****************
	// MIDDLE OBSTICLES
	// ****************

}

void Bomber::SpawnBomb()
{
	if (m_MaxAmountBombs >= m_Bombs.size())
	{
		GameObject* newBomb{ new GameObject };
		newBomb->AddComponent<ModelComponent>(new ModelComponent(L"BomberMan/Bombs/Bomb/model_ob001_bomb.ovm"));

		DiffuseMaterial* pBombMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		pBombMat->SetDiffuseTexture(L"BomberMan/Bombs/Bomb/tex_ob001_bomb_AL.png");
		newBomb->GetComponent<ModelComponent>()->SetMaterial(pBombMat);

		// snap on grid

		const float scale{ 5.f };
		XMFLOAT3 CharacterPos{ m_pCharacter->GetTransform()->GetPosition() };
		int xPos{ static_cast<int>((CharacterPos.x + (scale / 2.f)) / scale) };
		int zPos{ static_cast<int>((CharacterPos.z + (scale / 2.f)) / scale) };
		//std::cout << xPos << ", " << zPos << std::endl;
		//std::cout << (m_pCharacter->GetTransform()->GetPosition().x + (scale / 2.f)) << ", " << (m_pCharacter->GetTransform()->GetPosition().z + (scale / 2.f)) << std::endl;

		newBomb->GetTransform()->Scale(4.f);
		newBomb->GetTransform()->Rotate(90.f, 90.f, 0.f);
		newBomb->GetTransform()->Translate(scale * xPos, scale / 2.f, scale * zPos);
		AddChild(newBomb);

		Bomb newBombComplete{};
		newBombComplete.object = newBomb;
		newBombComplete.explosionLenght = m_ExplosionLenght;
		m_Bombs.push_back(newBombComplete);
	}
}

void Bomber::SpawnParticles(Bomb bomb)
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	XMFLOAT3 posBomb{ bomb.object->GetTransform()->GetPosition() };
	float scaleBomb{ bomb.object->GetTransform()->GetScale().x };

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,0.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 0.2f;
	settings.maxEnergy = 1.f;
	settings.minScale = 7.f;
	settings.maxScale = 9.f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f, 0.5f, 0.f, .8f };

	// middle particle
	TimedParticle particle{};
	GameObject* pObject{ new GameObject };
	pObject->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

	auto pCubeActor = pObject->AddComponent(new RigidBodyComponent(true));
	pCubeActor->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
	pObject->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				std::cout << "You Died" << std::endl;
				m_KillPlayer = true;
			}
		}
	);

	pObject->GetTransform()->Translate(posBomb);
	AddChild(pObject);

	particle.object = pObject;
	m_ActiveParticles.push_back(particle);



	for (int i = 0; i < bomb.explosionLenght; i++)
	{
		//TimedParticle particleSides{};
		//GameObject* pObjectSides{ new GameObject };
		//auto pCubeActorSides = pObjectSides->AddComponent(new RigidBodyComponent(true));
		//pObjectSides->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

		//pCubeActorSides->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		//pObjectSides->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
		//	{
		//		if (action == PxTriggerAction::ENTER)
		//		{
		//			std::cout << "You Died" << std::endl;
		//			m_KillPlayer = true;
		//		}
		//	}
		//);

		//switch (idx)
		//{
		//case 0:
		//	// right particle
		//	pObjectSides->GetTransform()->Translate(posBomb.x + (i * scaleBomb), posBomb.y, posBomb.z);
		//	break;
		//case 1:
		//	// left particle
		//	pObjectSides->GetTransform()->Translate(posBomb.x - (i * scaleBomb), posBomb.y, posBomb.z);
		//	break;
		//case 2:
		//	// up particle
		//	pObjectSides->GetTransform()->Translate(posBomb.x, posBomb.y, posBomb.z + (i * scaleBomb));
		//	break;
		//case 3:
		//	// down particle
		//	pObjectSides->GetTransform()->Translate(posBomb.x, posBomb.y, posBomb.z - (i * scaleBomb));
		//	break;
		//default:
		//	break;
		//}
		//AddChild(pObjectSides);

		//particleSides.object = pObjectSides;
		//m_ActiveParticles.push_back(particleSides);

		// right particle
		TimedParticle particleRight{};
		GameObject* pObjectSidesRight{ new GameObject };
		pObjectSidesRight->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

		auto pCubeActorRight = pObjectSidesRight->AddComponent(new RigidBodyComponent(true));
		pCubeActorRight->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesRight->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER)
				{
					std::cout << "You Died" << std::endl;
					m_KillPlayer = true;
				}
			}
		);

		pObjectSidesRight->GetTransform()->Translate(posBomb.x + (i * scaleBomb), posBomb.y, posBomb.z);
		AddChild(pObjectSidesRight);

		particleRight.object = pObjectSidesRight;
		m_ActiveParticles.push_back(particleRight);

		// left particle
		TimedParticle particleLeft{};
		GameObject* pObjectSidesLeft{ new GameObject };
		pObjectSidesLeft->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

		auto pCubeActorLeft = pObjectSidesLeft->AddComponent(new RigidBodyComponent(true));
		pCubeActorLeft->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesLeft->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER)
				{
					std::cout << "You Died" << std::endl;
					m_KillPlayer = true;
				}
			}
		);

		pObjectSidesLeft->GetTransform()->Translate(posBomb.x - (i * scaleBomb), posBomb.y, posBomb.z);
		AddChild(pObjectSidesLeft);

		particleLeft.object = pObjectSidesLeft;
		m_ActiveParticles.push_back(particleLeft);

		// up particle
		TimedParticle particleUp{};
		GameObject* pObjectSidesUp{ new GameObject };
		pObjectSidesUp->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

		auto pCubeActorUp = pObjectSidesUp->AddComponent(new RigidBodyComponent(true));
		pCubeActorUp->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesUp->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER)
				{
					std::cout << "You Died" << std::endl;
					m_KillPlayer = true;
				}
			}
		);

		pObjectSidesUp->GetTransform()->Translate(posBomb.x, posBomb.y, posBomb.z + (i * scaleBomb));
		AddChild(pObjectSidesUp);

		particleUp.object = pObjectSidesUp;
		m_ActiveParticles.push_back(particleUp);

		// down particle
		TimedParticle particleDown{};
		GameObject* pObjectSidesDown{ new GameObject };
		pObjectSidesDown->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

		auto pCubeActorDown = pObjectSidesDown->AddComponent(new RigidBodyComponent(true));
		pCubeActorDown->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesDown->SetOnTriggerCallBack([=](GameObject*, GameObject*, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER)
				{
					std::cout << "You Died" << std::endl;
					m_KillPlayer = true;
				}
			}
		);

		pObjectSidesDown->GetTransform()->Translate(posBomb.x, posBomb.y, posBomb.z - (i * scaleBomb));
		AddChild(pObjectSidesDown);

		particleDown.object = pObjectSidesDown;
		m_ActiveParticles.push_back(particleDown);
	}
}
