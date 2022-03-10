#pragma once

class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

private:
	void Reset();

	GameObject* m_pBall{};
	GameObject* m_pCubeLeft{};
	GameObject* m_pCubeRight{};

	GameObject* m_TriggerWallLeft{};
	GameObject* m_TriggerWallRight{};

	GameObject* m_WallTop{};
	GameObject* m_WallBottom{};

	float m_MovementSpeed{ 10.f };

	enum InputActions
	{
		RightPeddle_Up,
		RightPeddle_Down,
		LeftPeddle_Up,
		LeftPeddle_Down,
		LaunchBall,
		ResetButton,
	};
};

