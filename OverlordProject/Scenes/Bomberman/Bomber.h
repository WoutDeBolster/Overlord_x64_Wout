#pragma once
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
	Character* m_pCharacter{};
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb
	};

};

