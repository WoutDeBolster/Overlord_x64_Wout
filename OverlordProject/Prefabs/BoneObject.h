#pragma once

class BoneObject final : public GameObject
{
public:

	BoneObject(BaseMaterial* pMaterial, float lenght = 5.f);
	~BoneObject() override = default;
	BoneObject(const BoneObject& other) = delete;
	BoneObject(BoneObject&& other) noexcept = delete;
	BoneObject& operator=(const BoneObject& other) = delete;
	BoneObject& operator=(BoneObject&& other) noexcept = delete;

	void AddBone(BoneObject* pBone);

	// part 2
	const XMFLOAT4X4& GetBindPose() const { return m_BindPose; }
	void CalculateBindPose();

protected:
	void Initialize(const SceneContext&) override;

private:
	float m_Lenght{};
	BaseMaterial* m_pBaseMat{};

	// part 2
	XMFLOAT4X4 m_BindPose{};
};

