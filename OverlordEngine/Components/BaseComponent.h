#pragma once

class TransformComponent;
class GameObject;

class BaseComponent
{
public:
	BaseComponent() = default;
	virtual ~BaseComponent() = default;
	BaseComponent(const BaseComponent& other) = delete;
	BaseComponent(BaseComponent&& other) noexcept = delete;
	BaseComponent& operator=(const BaseComponent& other) = delete;
	BaseComponent& operator=(BaseComponent&& other) noexcept = delete;

	GameObject* GetGameObject() const { return m_pGameObject; }
	GameScene* GetScene() const { return m_pScene; }
	TransformComponent* GetTransform() const;

protected:

	virtual void Initialize(const SceneContext& sceneContext) = 0;
	virtual void PostInitialize(const SceneContext& /*sceneContext*/){}
	virtual void Update(const SceneContext& /*sceneContext*/){}
	virtual void Draw(const SceneContext& /*sceneContext*/){}
	virtual void PostDraw(const SceneContext&) {}
	virtual void OnOwnerAttach(GameObject* /*pOwner*/) {}
	virtual void OnOwnerDetach(GameObject* /*pPreviousOwner*/) {}
	virtual void OnSceneAttach(GameScene* /*pScene*/) {}
	virtual void OnSceneDetach(GameScene* /*pScene*/) {}

	GameObject* m_pGameObject{};
	GameScene* m_pScene{};
	bool m_IsInitialized{};

private:
	friend class GameObject;

	void RootInitialize(const SceneContext& sceneContext);
	void RootOnSceneAttach(GameScene*);
	void RootOnSceneDetach(GameScene*);
};


