#pragma once
#include "BaseComponent.h"

class MeshFilter;
class ModelAnimator;

class ModelComponent : public BaseComponent
{
public:
	ModelComponent(const std::wstring&  assetFile);
	~ModelComponent() override;

	ModelComponent(const ModelComponent& other) = delete;
	ModelComponent(ModelComponent&& other) noexcept = delete;
	ModelComponent& operator=(const ModelComponent& other) = delete;
	ModelComponent& operator=(ModelComponent&& other) noexcept = delete;

	void SetMaterial(BaseMaterial* pMaterial);
	void SetMaterial(UINT materialId);

	ModelAnimator* GetAnimator() const { return m_pAnimator; }
	bool HasAnimator() const { return m_pAnimator != nullptr; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext& sceneContext) override;

private:
	std::wstring m_AssetFile{};
	MeshFilter* m_pMeshFilter{};

	BaseMaterial* m_pMaterial{};
	bool m_MaterialChanged{};

	ModelAnimator* m_pAnimator{};
};
