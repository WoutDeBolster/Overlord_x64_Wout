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

#include "Materials/Post/PostGrayscale.h"
#include "Materials/Post/PostBlur.h"

Bomber::Bomber()
	: GameScene(L"BomberScene")
{
}

void Bomber::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");
	m_TextPosition = { m_SceneContext.windowWidth / 2.f - 15.f, m_SceneContext.windowHeight / 2.f };

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

	////Post Processing Stack
	////=====================
	//	m_pPostGrayscale = MaterialManager::Get()->CreateMaterial<PostGrayscale>();
	//
	//	AddPostProcessingEffect(m_pPostGrayscale);

	InitCharacter();
	InitLevel();
	InitSound();
	SpawnBreakebles();
}

void Bomber::Update()
{
	const float elapsed{ m_SceneContext.pGameTime->GetElapsed() };

	if (m_SceneContext.pInput->IsActionTriggered(CharacterPlaceBomb))
	{
		SpawnBomb();
	}

	// kill bomb
	for (size_t i = 0; i < m_Bombs.size(); i++)
	{
		m_Bombs[i].countDown -= elapsed;
		if (m_Bombs[i].countDown <= 0.f)
		{
			SpawnRaycasts(m_Bombs[i].object->GetTransform()->GetPosition());
			SpawnParticles(m_Bombs[i]);
			RemoveChild(m_Bombs[i].object);
			m_Bombs.erase(m_Bombs.begin() + i);
			SoundManager::Get()->GetSystem()->playSound(m_pSoundFx, m_pSoundEffectGroup, false, nullptr);
		}
	}
	m_TextBombsPlaced = "Bombs currently placed: " + std::to_string(m_Bombs.size());

	// kill particles
	for (size_t i = 0; i < m_ActiveParticles.size(); i++)
	{
		m_ActiveParticles[i].countDown -= elapsed;
		if (m_ActiveParticles[i].countDown <= 0.f)
		{
			RemoveChild(m_ActiveParticles[i].object);
			m_ActiveParticles.erase(m_ActiveParticles.begin() + i);
		}
	}

	if (m_KillPlayer)
	{
		TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Text), m_TextPosition, m_TextColor);
	}

	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_TextBombsPlaced), { 0.f, m_SceneContext.windowHeight - 30.f }, { 0.f ,0.f, 0.f, 1.f });

	// SOUND
	if (InputManager::IsKeyboardKey(InputState::pressed, VK_UP))
	{
		m_Volume += 0.1f;
		if (m_Volume > 1.f)
		{
			m_Volume = 1.f;
		}

		m_pSound2D->setVolume(m_Volume);
	}

	if (InputManager::IsKeyboardKey(InputState::pressed, VK_DOWN))
	{
		m_Volume -= 0.1f;
		if (m_Volume < 0.f)
		{
			m_Volume = 0.f;
		}

		m_pSound2D->setVolume(m_Volume);
	}
}

void Bomber::OnGUI()
{
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
	m_pCharacter->SetTag(L"Player");
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);
	const auto pModel = new ModelComponent(L"BomberMan/Ch09_nonPBR/Running.ovm");
	m_pCharacter->AddComponent<ModelComponent>(pModel);

	if (const auto pAnimator = pModel->GetAnimator())
	{
		pAnimator->SetAnimation(0);
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
	pGroundMaterial1->SetDiffuseTexture(L"BomberMan/Textures/ironGreen.jpg");
	const auto pGroundMaterial2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial2->SetDiffuseTexture(L"BomberMan/Textures/ironGrey.jpg");
	const auto pWallMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pWallMaterial1->SetDiffuseTexture(L"BomberMan/Textures/brickGrey.jpg");

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

				pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale) + 2.5f);
				pGroundObj->GetTransform()->Scale(scale);
				pLevelGrid->AddChild(pGroundObj);
			}
			else
			{
				const auto pGroundObj = new GameObject();

				const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
				pGroundModel->SetMaterial(pGroundMaterial2);
				pGroundObj->AddComponent(pGroundModel);

				pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), 0.f, static_cast<float>(y * scale) + 2.5f);
				pGroundObj->GetTransform()->Scale(scale);
				pLevelGrid->AddChild(pGroundObj);
			}
			ColorGreen = !ColorGreen;
		}
		ColorGreen = !ColorGreen;
	}

	const auto pColisionObj = new GameObject();
	auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
	pCubeActor->AddCollider(PxBoxGeometry{ (dimensions.x * scale * m_GridWidth) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale * m_GridHeight) / 2.f }, *pDefaultMaterial);
	pCubeActor->SetCollisionGroup(CollisionGroup::Group1);
	pCubeActor->GetTransform()->Translate((dimensions.x * scale * m_GridWidth - scale) / 2.f, 0.f, (dimensions.z * scale * m_GridHeight - scale) / 2.f);

	pLevelGrid->AddChild(pColisionObj);
	pLevelGrid->GetTransform()->Translate(0.f, 0.f, 0.f);
	AddChild(pLevelGrid);

	// ******
	// BORDER
	// ******
	GameObject* pLevelBorder{ new GameObject{} };
	int borderWidth{ m_GridWidth + 2 }; // +2 for the corners
	int borderHeight{ m_GridHeight }; // only one needs +2
	for (int x = 0; x < borderWidth; x++)
	{
		// bottom row
		const auto pWallObjBottom = new GameObject();

		const auto pWallModelBottom = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelBottom->SetMaterial(pWallMaterial1);
		pWallObjBottom->AddComponent(pWallModelBottom);

		pWallObjBottom->GetTransform()->Translate(static_cast<float>(x) - 1.f, 1.f, -dimensions.z + 0.5f);
		pLevelBorder->AddChild(pWallObjBottom);

		// top row
		const auto pWallObjTop = new GameObject();

		const auto pWallModelTop = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelTop->SetMaterial(pWallMaterial1);
		pWallObjTop->AddComponent(pWallModelTop);

		pWallObjTop->GetTransform()->Translate(static_cast<float>(x) - 1.f, 1.f, dimensions.z * m_GridHeight + 0.5f);
		pLevelBorder->AddChild(pWallObjTop);
	}
	for (int y = 0; y < borderHeight; y++)
	{
		// bottom row
		const auto pWallObjBottom = new GameObject();

		const auto pWallModelBottom = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelBottom->SetMaterial(pWallMaterial1);
		pWallObjBottom->AddComponent(pWallModelBottom);

		pWallObjBottom->GetTransform()->Translate(-dimensions.x, 1.f, static_cast<float>(y) + 0.5f);
		pLevelBorder->AddChild(pWallObjBottom);

		// top row
		const auto pWallObjTop = new GameObject();

		const auto pWallModelTop = new ModelComponent(L"Meshes/Cube.ovm");
		pWallModelTop->SetMaterial(pWallMaterial1);
		pWallObjTop->AddComponent(pWallModelTop);;

		pWallObjTop->GetTransform()->Translate(dimensions.x * m_GridWidth, 1.f, static_cast<float>(y) + 0.5f);
		pLevelBorder->AddChild(pWallObjTop);
	}

	const auto pColisionObjBottom = new GameObject();
	auto pCubeActorBottom = pColisionObjBottom->AddComponent(new RigidBodyComponent(true));
	pCubeActorBottom->AddCollider(PxBoxGeometry{ (dimensions.x * scale * m_GridWidth) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
	pCubeActorBottom->SetCollisionGroup(CollisionGroup::Group1);
	pCubeActorBottom->GetTransform()->Translate((dimensions.x * scale * m_GridWidth - scale) / 2.f, dimensions.y * scale, -dimensions.z * scale);

	const auto pColisionObjTop = new GameObject();
	auto pCubeActorTop = pColisionObjTop->AddComponent(new RigidBodyComponent(true));
	pCubeActorTop->AddCollider(PxBoxGeometry{ (dimensions.x * scale * m_GridWidth) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale) / 2.f }, *pDefaultMaterial);
	pCubeActorTop->SetCollisionGroup(CollisionGroup::Group1);
	pCubeActorTop->GetTransform()->Translate((dimensions.x * scale * m_GridWidth - scale) / 2.f, dimensions.y * scale, dimensions.z * scale * m_GridHeight);

	const auto pColisionObjLeft = new GameObject();
	auto pCubeActorLeft = pColisionObjLeft->AddComponent(new RigidBodyComponent(true));
	pCubeActorLeft->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale * m_GridHeight) / 2.f }, *pDefaultMaterial);
	pCubeActorLeft->SetCollisionGroup(CollisionGroup::Group1);
	pCubeActorLeft->GetTransform()->Translate((-dimensions.x * scale - scale) / 2.f, dimensions.y * scale, (dimensions.z * scale * m_GridHeight - scale) / 2.f);

	const auto pColisionObjRight = new GameObject();
	auto pCubeActorRight = pColisionObjRight->AddComponent(new RigidBodyComponent(true));
	pCubeActorRight->AddCollider(PxBoxGeometry{ (dimensions.x * scale) / 2.f, (dimensions.y * scale) / 2.f, (dimensions.z * scale * m_GridHeight) / 2.f }, *pDefaultMaterial);
	pCubeActorRight->SetCollisionGroup(CollisionGroup::Group1);
	pCubeActorRight->GetTransform()->Translate(dimensions.x * scale * m_GridWidth, dimensions.y * scale, (dimensions.z * scale * m_GridHeight - scale) / 2.f);

	pLevelBorder->AddChild(pColisionObjBottom);
	pLevelBorder->AddChild(pColisionObjTop);
	pLevelBorder->AddChild(pColisionObjLeft);
	pLevelBorder->AddChild(pColisionObjRight);


	pLevelBorder->GetTransform()->Translate(0.f, 0.f, 0.f);
	pLevelBorder->GetTransform()->Scale(scale);
	AddChild(pLevelBorder);

	// ****************
	// MIDDLE OBSTICLES
	// ****************

}

void Bomber::InitSound()
{
	//2D SOUND
	//========
	auto pFmodSystem = SoundManager::Get()->GetSystem();

	FMOD::Sound* pSound;
	auto fmodResult = pFmodSystem->createStream("Resources/BomberMan/Music/themeSong.mp3", FMOD_DEFAULT, nullptr, &pSound);

	pSound->setMode(FMOD_LOOP_NORMAL);

	m_pSound2D->setVolume(m_Volume);
	fmodResult = pFmodSystem->playSound(pSound, nullptr, false, &m_pSound2D);

	//Sound Effect
	fmodResult = pFmodSystem->createChannelGroup("Sound Effects", &m_pSoundEffectGroup);
	m_pSoundEffectGroup->setVolume(0.5f);
	fmodResult = pFmodSystem->createStream("Resources/BomberMan/Music/explosion-02.mp3", FMOD_DEFAULT, nullptr, &m_pSoundFx);
}

void Bomber::SpawnBomb()
{
	if (m_MaxAmountBombs >= m_Bombs.size())
	{
		const float scale{ 5.f };
		XMFLOAT3 CharacterPos{ m_pCharacter->GetTransform()->GetPosition() };
		int xPos{ static_cast<int>((CharacterPos.x + (scale / 2.f)) / scale) };
		int zPos{ static_cast<int>((CharacterPos.z + (scale / 2.f)) / scale) };

		GameObject* newBomb{ new GameObject };
		newBomb->AddComponent<ModelComponent>(new ModelComponent(L"BomberMan/Bombs/Bomb/model_ob001_bomb.ovm"));

		DiffuseMaterial* pBombMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
		pBombMat->SetDiffuseTexture(L"BomberMan/Bombs/Bomb/tex_ob001_bomb_AL.png");
		newBomb->GetComponent<ModelComponent>()->SetMaterial(pBombMat);

		// snap on grid
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
	float scaleBomb{ 5.f };

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
	pObject->AddComponent(new ParticleEmitterComponent(L"BomberMan/Textures/FireBall.png", settings, 100));

	auto pCubeActor = pObject->AddComponent(new RigidBodyComponent(true));
	pCubeActor->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
	pObject->SetOnTriggerCallBack([=](GameObject*, GameObject* object, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER && object->GetTag() == L"Player")
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
		// right particle
		TimedParticle particleRight{};
		GameObject* pObjectSidesRight{ new GameObject };
		pObjectSidesRight->AddComponent(new ParticleEmitterComponent(L"BomberMan/Textures/FireBall.png", settings, 100));

		auto pCubeActorRight = pObjectSidesRight->AddComponent(new RigidBodyComponent(true));
		pCubeActorRight->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesRight->SetOnTriggerCallBack([=](GameObject*, GameObject* object, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER && object->GetTag() == L"Player")
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
		pObjectSidesLeft->AddComponent(new ParticleEmitterComponent(L"BomberMan/Textures/FireBall.png", settings, 100));

		auto pCubeActorLeft = pObjectSidesLeft->AddComponent(new RigidBodyComponent(true));
		pCubeActorLeft->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesLeft->SetOnTriggerCallBack([=](GameObject*, GameObject* object, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER && object->GetTag() == L"Player")
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
		pObjectSidesUp->AddComponent(new ParticleEmitterComponent(L"BomberMan/Textures/FireBall.png", settings, 100));

		auto pCubeActorUp = pObjectSidesUp->AddComponent(new RigidBodyComponent(true));
		pCubeActorUp->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesUp->SetOnTriggerCallBack([=](GameObject*, GameObject* object, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER && object->GetTag() == L"Player")
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
		pObjectSidesDown->AddComponent(new ParticleEmitterComponent(L"BomberMan/Textures/FireBall.png", settings, 100));

		auto pCubeActorDown = pObjectSidesDown->AddComponent(new RigidBodyComponent(true));
		pCubeActorDown->AddCollider(PxBoxGeometry{ scaleBomb / 2.f, scaleBomb / 2.f, scaleBomb / 2.f }, *pDefaultMaterial, true);
		pObjectSidesDown->SetOnTriggerCallBack([=](GameObject*, GameObject* object, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER && object->GetTag() == L"Player")
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

void Bomber::SpawnBreakebles()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	const auto pWallMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pWallMaterial1->SetDiffuseTexture(L"BomberMan/Textures/brickGrey.jpg");
	const auto pGroundMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial1->SetDiffuseTexture(L"BomberMan/Textures/brickBrown.jpg");
	const auto pFinishMaterial1 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pFinishMaterial1->SetDiffuseTexture(L"BomberMan/Textures/flag_alb.png");

	float scale{ 5.f };
	XMFLOAT3 dim{ 1.f, 1.f, 1.f };
	for (int y = 0; y < m_GridHeight; y++)
	{
		for (int x = 0; x < m_GridWidth; x++)
		{
			if (y % 2 && y != 0)
			{
				if (x % 2)
				{
					const auto pGroundObj = new GameObject();
					const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
					pGroundModel->SetMaterial(pWallMaterial1);
					pGroundObj->AddComponent(pGroundModel);

					pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale + (scale / 2.f)));
					pGroundObj->GetTransform()->Scale(scale);

					const auto pColisionObj = new GameObject();
					auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
					pCubeActor->AddCollider(PxBoxGeometry{ scale / 2.f,  scale / 2.f,  scale / 2.f }, *pDefaultMaterial);
					pCubeActor->SetCollisionGroup(CollisionGroup::Group1);
					pCubeActor->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale));

					AddChild(pGroundObj);
					AddChild(pColisionObj);
				}
				else
				{
					if (y == 1 && x == 0)
					{
						// beginning gap
					}
					else
					{
						const auto pGroundObj = new GameObject();
						const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
						pGroundModel->SetMaterial(pGroundMaterial1);
						pGroundObj->AddComponent(pGroundModel);

						pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale + (scale / 2.f)));
						pGroundObj->GetTransform()->Scale(scale);

						const auto pColisionObj = new GameObject();
						pColisionObj->SetTag(L"break");
						auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
						pCubeActor->AddCollider(PxBoxGeometry{ scale / 2.f,  scale / 2.f,  scale / 2.f }, *pDefaultMaterial);
						pCubeActor->SetCollisionGroup(CollisionGroup::Group1);
						pCubeActor->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale));

						AddChild(pColisionObj);
						AddChild(pGroundObj);

						m_pBreakebleBlocks.push_back(Breakeble{ pGroundObj,pColisionObj });
					}
				}
			}
			else
			{
				if (y == (m_GridHeight - 1) && x == (m_GridWidth - 1))
				{
					const auto pGroundObj = new GameObject();
					const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
					pGroundModel->SetMaterial(pFinishMaterial1);
					pGroundObj->AddComponent(pGroundModel);

					pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale + (scale / 2.f)));
					pGroundObj->GetTransform()->Scale(scale);

					const auto pColisionObj = new GameObject();
					auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
					pCubeActor->AddCollider(PxBoxGeometry{ scale / 2.f,  scale / 2.f,  scale / 2.f }, *pDefaultMaterial, true);
					pCubeActor->SetCollisionGroup(CollisionGroup::Group1);
					pCubeActor->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale));
					pColisionObj->SetOnTriggerCallBack([=](GameObject*, GameObject* object, PxTriggerAction action)
						{

							if (action == PxTriggerAction::ENTER && object->GetTag() == L"Player")
							{
								std::cout << "You Win" << std::endl;
								m_Win = true;
							}
						}
					);

					AddChild(pGroundObj);
					AddChild(pColisionObj);
					m_pfinnish = pColisionObj;
				}
				else if (y == 0 && x == 0)
				{
					// beginning gap
				}
				else if (y == 0 && x == 1)
				{
					// beginning gap
				}
				else
				{
					const auto pGroundObj = new GameObject();
					const auto pGroundModel = new ModelComponent(L"Meshes/Cube.ovm");
					pGroundModel->SetMaterial(pGroundMaterial1);
					pGroundObj->AddComponent(pGroundModel);

					pGroundObj->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale + (scale / 2.f)));
					pGroundObj->GetTransform()->Scale(scale);

					const auto pColisionObj = new GameObject();
					pColisionObj->SetTag(L"break");
					auto pCubeActor = pColisionObj->AddComponent(new RigidBodyComponent(true));
					pCubeActor->AddCollider(PxBoxGeometry{ scale / 2.f,  scale / 2.f,  scale / 2.f }, *pDefaultMaterial);
					pCubeActor->SetCollisionGroup(CollisionGroup::Group1);
					pCubeActor->GetTransform()->Translate(static_cast<float>(x * scale), scale, static_cast<float>(y * scale));

					AddChild(pColisionObj);
					AddChild(pGroundObj);

					m_pBreakebleBlocks.push_back(Breakeble{ pGroundObj,pColisionObj });
				}
			}
		}
	}
}

void Bomber::SpawnRaycasts(XMFLOAT3 posBomb)
{
	const float scale{ 5.f };

	// up
	PxRaycastBuffer hitUp{};
	PxVec3 raydirectionUp{ 0.f, 0.f, 1.f };
	raydirectionUp.normalize();
	if (this->GetPhysxProxy()->Raycast({ posBomb.x, scale, posBomb.z }, raydirectionUp, scale, hitUp, PxHitFlag::eDEFAULT, PxQueryFilterData{}))
	{
		GameObject* pHitObj = reinterpret_cast<BaseComponent*>(hitUp.block.actor->userData)->GetGameObject();
		for (size_t i = 0; i < m_pBreakebleBlocks.size(); i++)
		{
			if (pHitObj == m_pBreakebleBlocks[i].collision)
			{
				RemoveChild(m_pBreakebleBlocks[i].model);
				RemoveChild(m_pBreakebleBlocks[i].collision);
				m_pBreakebleBlocks.erase(m_pBreakebleBlocks.begin() + i);
				//std::cout << "Up";
			}
		}
	}

	// down
	PxRaycastBuffer hitDown{};
	PxVec3 raydirectionDown{ 0.f, 0.f, -1.f };
	raydirectionDown.normalize();
	if (this->GetPhysxProxy()->Raycast({ posBomb.x, scale, posBomb.z }, raydirectionDown, scale, hitDown, PxHitFlag::eDEFAULT, PxQueryFilterData{}))
	{
		GameObject* pHitObj = reinterpret_cast<BaseComponent*>(hitDown.block.actor->userData)->GetGameObject();
		for (size_t i = 0; i < m_pBreakebleBlocks.size(); i++)
		{
			if (pHitObj == m_pBreakebleBlocks[i].collision)
			{
				RemoveChild(m_pBreakebleBlocks[i].model);
				RemoveChild(m_pBreakebleBlocks[i].collision);
				m_pBreakebleBlocks.erase(m_pBreakebleBlocks.begin() + i);
				//std::cout << "Down";
			}
		}
	}

	// right
	PxRaycastBuffer hitRight{};
	PxVec3 raydirectionRight{ 1.f, 0.f, 0.f };
	raydirectionRight.normalize();
	if (this->GetPhysxProxy()->Raycast({ posBomb.x, scale, posBomb.z }, raydirectionRight, scale, hitRight, PxHitFlag::eDEFAULT, PxQueryFilterData{}))
	{
		GameObject* pHitObj = reinterpret_cast<BaseComponent*>(hitRight.block.actor->userData)->GetGameObject();
		for (size_t i = 0; i < m_pBreakebleBlocks.size(); i++)
		{
			if (pHitObj == m_pBreakebleBlocks[i].collision)
			{
				RemoveChild(m_pBreakebleBlocks[i].model);
				RemoveChild(m_pBreakebleBlocks[i].collision);
				m_pBreakebleBlocks.erase(m_pBreakebleBlocks.begin() + i);
				//std::cout << "Right";
			}
		}
	}

	// left
	PxRaycastBuffer hitLeft{};
	PxVec3 raydirectionLeft{ -1.f, 0.f, 0.f };
	raydirectionLeft.normalize();
	if (this->GetPhysxProxy()->Raycast({ posBomb.x, scale, posBomb.z }, raydirectionLeft, scale, hitLeft, PxHitFlag::eDEFAULT, PxQueryFilterData{}))
	{
		if (hitRight.block.actor != nullptr)
		{
			GameObject* pHitObj = reinterpret_cast<BaseComponent*>(hitRight.block.actor->userData)->GetGameObject();
			for (size_t i = 0; i < m_pBreakebleBlocks.size(); i++)
			{
				if (pHitObj == m_pBreakebleBlocks[i].collision)
				{
					RemoveChild(m_pBreakebleBlocks[i].model);
					RemoveChild(m_pBreakebleBlocks[i].collision);
					m_pBreakebleBlocks.erase(m_pBreakebleBlocks.begin() + i);
					//std::cout << "Left";
				}
			}
		}
	}
}
