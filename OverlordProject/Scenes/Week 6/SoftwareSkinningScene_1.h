#pragma once
class BoneObject;

class SoftwareSkinningScene_1 final : public GameScene
{
public:
	SoftwareSkinningScene_1() :GameScene(L"SoftwareSkinningScene_1") {}
	~SoftwareSkinningScene_1() override = default;
	SoftwareSkinningScene_1(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1(SoftwareSkinningScene_1&& other) noexcept = delete;
	SoftwareSkinningScene_1& operator=(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1& operator=(SoftwareSkinningScene_1&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	BoneObject* m_pBone0{};
	BoneObject* m_pBone1{};

	bool RotateUp{ true };
	float m_BoneRotation0{};
	float m_BoneRotation1{};
	int m_RotationSign{ 1 };

	// imgui
	bool m_ManualRotate{ false };
};

