#pragma once
#include "Materials/UberMaterial.h"
#include "Materials/DiffuseMaterial.h"

class Character;

class Portal : public GameScene
{
public:
	Portal();
	~Portal() override = default;

	Portal(const Portal& other) = delete;
	Portal(Portal&& other) noexcept = delete;
	Portal& operator=(const Portal& other) = delete;
	Portal& operator=(Portal&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

private:
	// portals
	bool bluePortal{ false };
	bool OrangePortal{ false };

	//CameraComponent* m_pBlueCam = nullptr;
	//CameraComponent* m_pOrangeCam = nullptr;

	// functions
	void InitPortals();
	void InitCharater();
	void InitObjects();

	// character
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		CharacterPickUpItem,
	};

	Character* m_pCharacter{};

	// objects
	GameObject* m_pCube = nullptr;

	GameObject* m_pBluePortal = nullptr;
	GameObject* m_pOrangePortal = nullptr;

	GameObject* m_pPortalGun = nullptr;

	// mats
	DiffuseMaterial* m_pCubeMat = nullptr;
	DiffuseMaterial* m_pCharacterMat = nullptr;
	DiffuseMaterial* m_pBluePortalMat = nullptr;
	DiffuseMaterial* m_pOrangePortalMat = nullptr;
	DiffuseMaterial* m_pPortalGunMat = nullptr;
};

