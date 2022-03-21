#pragma once

class UberMaterial final : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() override = default;

	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

	void UseAlphaBlending(bool enabled);

	//DIFFUSE
	//*******
	void SetDiffuseTexture(const std::wstring& assetFile);

	//SPECULAR
	//********
	void SetSpecularLevelTexture(const std::wstring& assetFile);

	//NORMAL MAPPING
	//**************
	void SetNormalTexture(const std::wstring& assetFile);


	//ENVIRONMENT MAPPING
	//*******************
	void SetEnvironmentCube(const std::wstring& assetFile);

	//OPACITY
	//*******
	void SetOpacityMap(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;

	TextureData* m_pDiffuseTexture = nullptr;
	TextureData* m_pSpecularLevelTexture = nullptr;
	TextureData* m_pNormalTexture = nullptr;
	TextureData* m_pCubeMap = nullptr;
	TextureData* m_pOpacityMap = nullptr;
};

