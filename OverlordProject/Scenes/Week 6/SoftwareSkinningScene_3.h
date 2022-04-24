#pragma once
class BoneObject;

class SoftwareSkinningScene_3 final : public GameScene
{
public:
	SoftwareSkinningScene_3() :GameScene(L"SoftwareSkinningScene_3") {}
	~SoftwareSkinningScene_3() override = default;
	SoftwareSkinningScene_3(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3(SoftwareSkinningScene_3&& other) noexcept = delete;
	SoftwareSkinningScene_3& operator=(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3& operator=(SoftwareSkinningScene_3&& other) noexcept = delete;

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

	// part 2
	struct VertexSoftwareSkinned
	{
		VertexSoftwareSkinned(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color, float blendweightbone0, float blendweightbone1)
			: transformedVertex{ position, normal, color }
			, originalVertex{ position, normal, color }
			, blendWeight0{ blendweightbone0 }
			, blendWeight1{ blendweightbone1 }
		{

		}

		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};

		float blendWeight0{};
		float blendWeight1{};
	};

	void InitializeVertices(float length);

	MeshDrawComponent* m_MeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{};
};

