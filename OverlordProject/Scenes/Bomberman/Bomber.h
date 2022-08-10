#pragma once
class Character;

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

private:
	void InitCharacter();
	void InitLevel();
	void SpawnBomb();
	void SpawnParticles(Bomb bomb);

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

	int m_GridWidth{ 8 };
	int m_GridHeight{ 8 };

	std::vector<Bomb> m_Bombs;
	std::vector<TimedParticle> m_ActiveParticles;
	int m_MaxAmountBombs{ 10 };
	int m_ExplosionLenght{ 1 };
};

