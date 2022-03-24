#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial() :
	Material<UberMaterial>(L"Effects/UberShader.fx")
{
}

void UberMaterial::UseAlphaBlending(bool enabled)
{
	SetTechnique(enabled ? L"WithAlphaBlending" : L"WithoutAlphaBlending");
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial::SetSpecularLevelTexture(const std::wstring& assetFile)
{
	m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularLevelTexture);
}

void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}

void UberMaterial::SetEnvironmentCube(const std::wstring& assetFile)
{
	m_pCubeMap = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gCubeEnvironment", m_pCubeMap);
}

void UberMaterial::SetOpacityMap(const std::wstring& assetFile)
{
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureOpacity", m_pOpacityMap);
}

void UberMaterial::InitializeEffectVariables()
{
	//DIFFUSE
	//*******
	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Vector(L"gColorDiffuse", { XMFLOAT3(Colors::Red) });
	SetDiffuseTexture(L"Textures/Skulls Textures/Skulls_Diffusemap.tga");

	//SPECULAR
	//********
	SetVariable_Vector(L"gColorSpecular", { XMFLOAT3(Colors::Yellow) });
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", false);
	SetVariable_Scalar(L"gShininess", 40);
	SetSpecularLevelTexture(L"Textures/Skulls Textures/Skulls_Heightmap.tga");

	//AMBIENT
	//*******
	SetVariable_Vector(L"gColorAmbient", { XMFLOAT3(Colors::Black) });
	SetVariable_Scalar(L"gAmbientIntensity", 0);

	//NORMAL MAPPING
	//**************
	SetVariable_Scalar(L"gFlipGreenChannel", true);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetNormalTexture(L"Textures/Skulls Textures/Skulls_Normalmap.tga");

	//ENVIRONMENT MAPPING
	//*******************
	SetVariable_Scalar(L"gUseEnvironmentMapping", true);
	SetVariable_Scalar(L"gReflectionStrength", 0.8f);
	SetVariable_Scalar(L"gRefractionStrength", 0.5f);
	SetVariable_Scalar(L"gRefractionIndex", 0.9f);
	SetEnvironmentCube(L"Textures/Sunol_Cubemap.dds");

	//FRESNEL FALLOFF
	//***************
	SetVariable_Scalar(L"gUseFresnelFalloff", false);
	SetVariable_Vector(L"gColorFresnel", { XMFLOAT3(Colors::Black) });
	SetVariable_Scalar(L"gFresnelPower", 2);
	SetVariable_Scalar(L"gFresnelMultiplier", 1.5f);
	SetVariable_Scalar(L"gFresnelHardness", 2);

	//OPACITY
	//*******
	SetVariable_Scalar(L"gOpacityIntensity", 1);
	SetVariable_Scalar(L"gTextureOpacityIntensity", false);
	SetOpacityMap(L"Textures/Skulls Textures/Skulls_Heightmap.tga");

	//SPECULAR MODELS
	//***************
	SetVariable_Scalar(L"gUseSpecularBlinn", false);
	SetVariable_Scalar(L"gUseSpecularPhong", true);
}

