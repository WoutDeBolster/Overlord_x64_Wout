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
	GameObject* m_pBall{};
	GameObject* m_pCubeLeft{};
	GameObject* m_pCubeRight{};

	enum InputActions
	{
		RightPeddle_Up,
		RightPeddle_Down,
		LeftPeddle_Up,
		LeftPeddle_Down,
		LaunchBall,
	};
};

