#pragma once
class BaseMaterial;

class MaterialManager final : public Singleton<MaterialManager>
{
public:
	MaterialManager(const MaterialManager& other) = delete;
	MaterialManager(MaterialManager&& other) noexcept = delete;
	MaterialManager& operator=(const MaterialManager& other) = delete;
	MaterialManager& operator=(MaterialManager&& other) noexcept = delete;

	template<typename T>
	T* CreateMaterial();

	template<typename T>
	T* GetMaterial(UINT materialId) const;
	BaseMaterial* GetMaterial(UINT materialId) const;

	void RemoveMaterial(UINT materialId, bool deleteObj = false);
	void RemoveMaterial(BaseMaterial* pMaterial, bool deleteObj = false);


protected:
	void Initialize() override {};

private:
	friend Singleton<MaterialManager>;
	MaterialManager() = default;
	~MaterialManager();

	static bool IsValid(UINT id) { return id != UINT_MAX; }
	std::vector<BaseMaterial*> m_Materials{};
};

template <typename T>
T* MaterialManager::CreateMaterial()
{
	auto pMaterial = new T();

	UINT newMaterialId{ UINT_MAX};
	for (size_t i{ 0 }; i < m_Materials.size(); ++i)
	{
		if (m_Materials[i] == nullptr)
		{
			newMaterialId = UINT(i);
			break;
		}
	}

	if (newMaterialId == UINT_MAX)
	{
		newMaterialId = UINT(m_Materials.size());
		m_Materials.push_back(pMaterial);
	}
	else m_Materials[newMaterialId] = pMaterial;

	pMaterial->SetMaterialName(StringUtil::utf8_decode(typeid(T).name()));
	pMaterial->Initialize(m_GameContext.d3dContext, newMaterialId);

	return pMaterial;
}

template <typename T>
T* MaterialManager::GetMaterial(UINT materialId) const
{
	if(auto pBase = GetMaterial(materialId))
	{
		if(T * pDerived = dynamic_cast<T>(pBase))
		{
			return pDerived;
		}

		CHECK_ERROR_ARG(L"Failed to cast BaseMaterial to \'{}\'", StringUtil::utf8_decode(typeid(T).name()));
	}

	return nullptr;
}

