#pragma once
class SpriteMaterial : public Material<SpriteMaterial>
{
public:
	SpriteMaterial();
	~SpriteMaterial() override = default;

	SpriteMaterial(const SpriteMaterial& other) = delete;
	SpriteMaterial(SpriteMaterial&& other) noexcept = delete;
	SpriteMaterial& operator=(const SpriteMaterial& other) = delete;
	SpriteMaterial& operator=(SpriteMaterial&& other) noexcept = delete;

	// sprite
	void SetSpriteTexture(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pSpriteTexture = nullptr;
};

