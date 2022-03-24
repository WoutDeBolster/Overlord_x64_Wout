#include "stdafx.h"
#include "SpikyScene.h"

SpikyScene::SpikyScene()
	:GameScene(L"SpikyScene")
{
}

void SpikyScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	m_pSphere = AddChild(new GameObject());
	m_pSphere->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	m_pSphere->GetComponent<ModelComponent>()->GetTransform()->Scale(10.f);

	m_pSpikyMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();
	m_pSphere->GetComponent<ModelComponent>()->SetMaterial(m_pSpikyMaterial);
}

void SpikyScene::Update()
{
	float totalTime = m_SceneContext.pGameTime->GetTotal();
	m_pSphere->GetTransform()->Rotate(0.f, 5.f * totalTime, 0.f);
}

void SpikyScene::OnGUI()
{
	m_pSpikyMaterial->DrawImGui();
}
