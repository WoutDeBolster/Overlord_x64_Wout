#pragma once
struct VertexBufferData
{
	VertexBufferData():
		pDataStart(nullptr),
		pVertexBuffer(nullptr),
		BufferSize(0),
		VertexStride(0),
		VertexCount(0),
		IndexCount(0),
		InputLayoutID(0){}

	void* pDataStart;
	ID3D11Buffer* pVertexBuffer;
	UINT BufferSize;
	UINT VertexStride;
	UINT VertexCount;
	UINT IndexCount;
	UINT InputLayoutID;

	void Destroy()
	{
		free(pDataStart);
		SafeRelease(pVertexBuffer);
	}
};

struct AnimationKey
{
	float tick = {};
	std::vector<DirectX::XMFLOAT4X4> boneTransforms = {};
};

struct AnimationClip
{
	AnimationClip() = default;

	std::wstring name = {};
	float duration = {};
	float ticksPerSecond = {};
	std::vector<AnimationKey> keys = {};
};

class MeshFilter final
{
public:
	MeshFilter() = default;
	~MeshFilter();
	MeshFilter(const MeshFilter& other) = delete;
	MeshFilter(MeshFilter&& other) noexcept = delete;
	MeshFilter& operator=(const MeshFilter& other) = delete;
	MeshFilter& operator=(MeshFilter&& other) noexcept = delete;

private:
	friend class MeshFilterLoader;
	friend class ModelComponent;
	friend class ModelAnimator;

	int GetVertexBufferId(UINT inputLayoutId) const;
	void BuildVertexBuffer(const SceneContext& gameContext, BaseMaterial* pMaterial);
	void BuildIndexBuffer(const SceneContext& gameContext);

	void SetElement(ILSemantic element) { m_Elements |= element; }
	bool HasElement(ILSemantic element) const { return isSet(m_Elements, element); }

	const VertexBufferData& GetVertexBufferData(const SceneContext& sceneContext, BaseMaterial* pMaterial);

	//VERTEX DATA
	UINT m_VertexCount{}, m_IndexCount{}, m_TexCoordCount{};
	ILSemantic m_Elements{ILSemantic::NONE};
	std::vector<XMFLOAT3> m_Positions{};
	std::vector<XMFLOAT3> m_Normals{};
	std::vector<XMFLOAT3> m_Tangents{};
	std::vector<XMFLOAT3> m_Binormals{};
	std::vector<XMFLOAT2> m_TexCoords{};
	std::vector<XMFLOAT4> m_Colors{}; //W7
	std::vector<XMFLOAT4> m_BlendIndices{};
	std::vector<XMFLOAT4> m_BlendWeights{};
	std::vector<AnimationClip> m_AnimationClips{};
	bool m_HasAnimations{};
	USHORT m_BoneCount{};

	//INDEX DATA
	std::vector<DWORD> m_Indices{};

	std::vector<VertexBufferData> m_VertexBuffers{};
	ID3D11Buffer* m_pIndexBuffer{};

	std::wstring m_MeshName{};
	static XMFLOAT4 m_DefaultColor;
	static XMFLOAT4 m_DefaultFloat4;
	static XMFLOAT3 m_DefaultFloat3;
	static XMFLOAT2 m_DefaultFloat2;
};

