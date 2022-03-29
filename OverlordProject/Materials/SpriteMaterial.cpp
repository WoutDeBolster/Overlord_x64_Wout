#include "stdafx.h"
#include "SpriteMaterial.h"

SpriteMaterial::SpriteMaterial()
	:Material<SpriteMaterial>(L"Effects/SpriteRenderer.fx")
{
}

void SpriteMaterial::SetSpriteTexture(const std::wstring& assetFile)
{
	m_pSpriteTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gSpriteTexture", m_pSpriteTexture);
}

void SpriteMaterial::InitializeEffectVariables()
{
	//SetVariable_Scalor(L"gTextureSize", { XMFLOAT2(1.f, 1.f) });
	SetSpriteTexture(L"Textures/TestSprite.tga");
}
