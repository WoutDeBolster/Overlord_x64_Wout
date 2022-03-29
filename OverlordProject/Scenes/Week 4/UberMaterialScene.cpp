#include "stdafx.h"
#include "UberMaterialScene.h"

UberMaterialScene::UberMaterialScene()
	: GameScene(L"UberMaterialScene")
{
}

void UberMaterialScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	m_pSphere = AddChild(new GameObject());
	m_pSphere->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Sphere.ovm"));
	m_pSphere->GetComponent<ModelComponent>()->GetTransform()->Scale(10.f);

	m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pSphere->GetComponent<ModelComponent>()->SetMaterial(m_pUberMaterial);
}

void UberMaterialScene::Update()
{

}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}
