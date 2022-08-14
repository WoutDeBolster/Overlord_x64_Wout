#pragma once
class Character;
class PostGrayscale;

struct Bomb
{
	float countDown{ 3.f };
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
	std::vector<TimedParticle> m_ActiveParticles;
	std::vector<Breakeble> m_pBreakebleBlocks;
	GameObject* m_pfinnish;
	int m_MaxAmountBombs{ 10 };
	int m_ExplosionLenght{ 2 };

	// text
	SpriteFont* m_pFont{};

	std::string m_Text{ "YOU DIED" };
	std::string m_TextBombsPlaced{};
	std::string m_GameTimer{};
	XMFLOAT2 m_TextPosition{};
	XMFLOAT4 m_TextColor{ 1.f,0.f,0.f,1.f };

	//sound
	FMOD::Channel* m_pSound2D{};
	float m_Volume{ 1.f };

	FMOD::Sound* m_pSoundFx{};
	FMOD::ChannelGroup* m_pSoundEffectGroup{};

	// post
	PostGrayscale* m_pPostGrayscale{};
};

