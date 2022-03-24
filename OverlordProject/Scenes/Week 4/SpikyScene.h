#pragma once

#include "Materials/SpikyMaterial.h"

class SpikyScene final : public GameScene
{
public:
	SpikyScene();
	~SpikyScene() override = default;

	SpikyScene(const SpikyScene& other) = delete;
	SpikyScene(SpikyScene&& other) noexcept = delete;
	SpikyScene& operator=(const SpikyScene& other) = delete;
	SpikyScene& operator=(SpikyScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	GameObject* m_pSphere = nullptr;
	SpikyMaterial* m_pSpikyMaterial = nullptr;
};

