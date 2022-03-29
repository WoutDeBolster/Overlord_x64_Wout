#include "stdafx.h"
#include "ModelTestScene.h"

ModelTestScene::ModelTestScene()
	: GameScene(L"ModelTestScene")
{
}

void ModelTestScene::Initialize()
{
	m_pChair = AddChild(new GameObject());
	m_pChair->AddComponent<ModelComponent>(new ModelComponent(L"Meshes/Chair.ovm"));
	//m_pChair->GetComponent<ModelComponent>()->GetTransform()->Scale(10.f);

	m_pDiffMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pChair->GetComponent<ModelComponent>()->SetMaterial(m_pDiffMat);
}

void ModelTestScene::Update()
{
}
