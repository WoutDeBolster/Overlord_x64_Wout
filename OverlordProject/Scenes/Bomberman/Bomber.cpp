#include "stdafx.h"
#include "Bomber.h"

#include "Prefabs/Character.h"
#include "Prefabs/CubePrefab.h"

#include "Materials/ColorMaterial.h"
#include "Materials/UberMaterial.h"
#include "Materials/DiffuseMaterial.h"

Bomber::Bomber()
	: GameScene(L"BomberScene")
{
}

void Bomber::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	// cam top down
	auto camEmpty = new GameObject();
	auto cam = new CameraComponent();
	camEmpty->GetTransform()->Translate(0.f, 100.f, -40.f);
	camEmpty->GetTransform()->Rotate(65.f, 0.f, 0.f);
	camEmpty->AddComponent(cam);
	AddChild(camEmpty);
	SetActiveCamera(cam);

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	InitCharacter();
	InitLevel();
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
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterPlaceBomb;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);
	m_pCharacter->AddComponent<ModelComponent>(new ModelComponent(L"BomberMan/Ch09_nonPBR/Ch09_nonPBR.ovm"));

	DiffuseMaterial* pCharaterMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pCharaterMat->SetDiffuseTexture(L"BomberMan/Ch09_nonPBR/Ch09_1001_Diffuse.png");
	m_pCharacter->GetComponent<ModelComponent>()->SetMaterial(pCharaterMat);
	//m_pCharacter->GetComponent<ModelComponent>()->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
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
				CubePrefab* newCube{ new CubePrefab(dimensions, forestGreen) };
				newCube->GetTransform()->Translate(static_cast<float>(x), 0.f, static_cast<float>(y));
				auto pCubeActor = newCube->AddComponent(new RigidBodyComponent(true));
				pCubeActor->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
				pCubeActor->SetCollisionGroup(CollisionGroup::Group1);

				pCubeActor->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale));
				pCubeActor->GetTransform()->Scale(scale);
				pLevelGrid->AddChild(newCube);
			}
			else
			{
				CubePrefab* newCube{ new CubePrefab(dimensions, lightGray) };
				newCube->GetTransform()->Translate(static_cast<float>(x), 0.f, static_cast<float>(y));
				auto pCubeActor = newCube->AddComponent(new RigidBodyComponent(true));
				pCubeActor->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
				pCubeActor->SetCollisionGroup(CollisionGroup::Group1);

				pCubeActor->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale));
				pCubeActor->GetTransform()->Scale(scale);
				pLevelGrid->AddChild(newCube);
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
		CubePrefab* newCubeBottom{ new CubePrefab(dimensions, darkGrey) };
		auto pCubeActorBottom = newCubeBottom->AddComponent(new RigidBodyComponent(true));
		pCubeActorBottom->AddCollider(PxBoxGeometry{ dimensions.x / 2.f, dimensions.y / 2.f, dimensions.z / 2.f }, *pDefaultMaterial);
		pCubeActorBottom->SetCollisionGroup(CollisionGroup::Group1);

		newCubeBottom->GetTransform()->Translate(static_cast<float>(x) - 1.f, 1.f, -dimensions.z);
		pLevelBorder->AddChild(newCubeBottom);

		// top row
		CubePrefab* newCubeTop{ new CubePrefab(dimensions, darkGrey) };
		auto pCubeActorTop = newCubeTop->AddComponent(new RigidBodyComponent(true));
		pCubeActorTop->AddCollider(PxBoxGeometry{ dimensions.x / 2.f, dimensions.y / 2.f, dimensions.z / 2.f }, *pDefaultMaterial);
		pCubeActorTop->SetCollisionGroup(CollisionGroup::Group1);

		newCubeTop->GetTransform()->Translate(static_cast<float>(x) - 1.f, 1.f, dimensions.z * m_GridHeight);
		pLevelBorder->AddChild(newCubeTop);
	}
	for (int y = 0; y < borderHeight; y++)
	{
		// bottom row
		CubePrefab* newCubeBottom{ new CubePrefab(dimensions, darkGrey) };
		auto pCubeActorBottom = newCubeBottom->AddComponent(new RigidBodyComponent(true));
		pCubeActorBottom->AddCollider(PxBoxGeometry{ dimensions.x / 2.f, dimensions.y / 2.f, dimensions.z / 2.f }, *pDefaultMaterial);
		pCubeActorBottom->SetCollisionGroup(CollisionGroup::Group1);

		newCubeBottom->GetTransform()->Translate(-dimensions.x, 1.f, static_cast<float>(y));
		pLevelBorder->AddChild(newCubeBottom);

		// top row
		CubePrefab* newCubeTop{ new CubePrefab(dimensions, darkGrey) };
		auto pCubeActorTop = newCubeTop->AddComponent(new RigidBodyComponent(true));
		pCubeActorTop->AddCollider(PxBoxGeometry{ dimensions.x / 2.f, dimensions.y / 2.f, dimensions.z / 2.f }, *pDefaultMaterial);
		pCubeActorTop->SetCollisionGroup(CollisionGroup::Group1);

		newCubeTop->GetTransform()->Translate(dimensions.x * m_GridWidth, 1.f, static_cast<float>(y));
		pLevelBorder->AddChild(newCubeTop);
	}
	pLevelBorder->GetTransform()->Translate(0.f, 0.f, 0.f);
	pLevelBorder->GetTransform()->Scale(5.f);
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
		for (size_t idx = 0; idx < 4; idx++)
		{
			//TimedParticle particleSides{};
			//GameObject* pObjectSides{ new GameObject };
			//pObjectSides->AddComponent(new ParticleEmitterComponent(L"BomberMan/FireBall.png", settings, 100));

			//auto pCubeActorSides = pObjectSides->AddComponent(new RigidBodyComponent(true));
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

			pObjectSidesRight->GetTransform()->Translate(posBomb.x + scaleBomb, posBomb.y, posBomb.z);
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

			pObjectSidesLeft->GetTransform()->Translate(posBomb.x - scaleBomb, posBomb.y, posBomb.z);
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

			pObjectSidesUp->GetTransform()->Translate(posBomb.x, posBomb.y, posBomb.z + scaleBomb);
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

			pObjectSidesDown->GetTransform()->Translate(posBomb.x, posBomb.y, posBomb.z - scaleBomb);
			AddChild(pObjectSidesDown);

			particleDown.object = pObjectSidesDown;
			m_ActiveParticles.push_back(particleDown);
		}
	}
}
