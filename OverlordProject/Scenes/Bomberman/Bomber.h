#pragma once
class Character;
class PostGrayscale;
class PostBlur;

struct Bomb
{
	float countDown{ 2.f };
	int explosionLenght{ 1 };
	GameObject* object{ nullptr };
};

struct TimedParticle
{
	float countDown{ 1.f };
	GameObject* object{ nullptr };
};

struct Breakeble
{
	GameObject* model{ nullptr };
	GameObject* collision{ nullptr };
};

class Bomber : public GameScene
{
public:
	Bomber();
	~Bomber() override = default;

	Bomber(const Bomber& other) = delete;
	Bomber(Bomber&& other) noexcept = delete;
	Bomber& operator=(const Bomber& other) = delete;
	Bomber& operator=(Bomber&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	void InitCharacter();
	void InitLevel();
	void InitSound();
	void SpawnBomb();
	void SpawnParticles(Bomb bomb);
	void SpawnBreakebles();
	void SpawnRaycasts(XMFLOAT3 posBomb);
	void MainMenu();
	void PauzeMenu();

	Character* m_pCharacter{};
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb
	};
	bool m_KillPlayer{ false };
	bool m_Win{ false };

	int m_GridWidth{ 7 };
	int m_GridHeight{ 5 };

	std::vector<Bomb> m_Bombs;
	GameObject* m_pLevelGrid{ new GameObject() };
	GameObject* m_pLevelBorder{ new GameObject() };
	std::vector<TimedParticle> m_ActiveParticles;
	std::vector<Breakeble> m_pBreakebleBlocks;
	GameObject* m_pfinnish{};
	CameraComponent* m_pMainCam{};
	int m_MaxAmountBombs{ 3 };
	int m_ExplosionLenght{ 2 };

	// menu
	std::vector<GameObject*> m_pMainMenu;
	bool m_loadMainMenu{ true };
	bool m_MenuOn{ true };
	std::vector<GameObject*> m_pPauzeMenu;
	bool m_loadPauzeMenu{ false };
	bool m_GamePauzed{ false };

	float m_Time{ 100.f };

	// text
	SpriteFont* m_pFont{};

	std::string m_TextBombsPlaced{};
	std::string m_GameTimer{};
	std::string m_CurrentVolume{};
	XMFLOAT2 m_TextPosition{};

	//sound
	FMOD::Channel* m_pSound2D{};
	float m_Volume{ 1.f };

	FMOD::Sound* m_pSoundFx{};
	FMOD::ChannelGroup* m_pSoundEffectGroup{};

	// post
	PostGrayscale* m_pPostGrayscale{};
	PostBlur* m_pPostBlur{};
	bool m_PostGrayscaleAdded{ false };
	bool m_PostBlurAdded{ false };
};

