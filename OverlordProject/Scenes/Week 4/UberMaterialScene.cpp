#include "stdafx.h"
#include "UberMaterialScene.h"

UberMaterialScene::UberMaterialScene()
	: GameScene(L"UberMaterialScene")
{
}

void UberMaterialScene::Initialize()
{
	m_pUberMaterial = new UberMaterial();
	//m_pUberMaterial->SetDiffuseTexture()
}

void UberMaterialScene::Update()
{
}
