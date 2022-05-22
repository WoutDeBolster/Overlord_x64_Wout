#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount) :
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO_W9(L"Implement Destructor")
	delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Initialize")
	m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement CreateVertexBuffer")
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_MaxParticles * sizeof(VertexParticle);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pVertexBuffer));
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update")
	const float elapsedTime{ sceneContext.pGameTime->GetElapsed() };
	float particleInterval{ (m_EmitterSettings.minEnergy + m_EmitterSettings.maxEnergy) / 2.f };
	particleInterval /= m_ParticleCount;
	m_LastParticleSpawn += elapsedTime;

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VertexParticle* pParticle = static_cast<VertexParticle*>(mappedResource.pData);

	for (UINT i{}; i < m_ParticleCount; i++)
	{
		if (m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], elapsedTime);
		}

		if (!m_ParticlesArray[i].isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(m_ParticlesArray[i]);
		}

		if (m_ParticlesArray[i].isActive)
		{
			pParticle[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;
			++m_ActiveParticles;
		}
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")
	if (!p.isActive)
	{
		return;
	}

	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy <= 0.f)
	{
		p.isActive = false;
		return;
	}

	// pos
	XMFLOAT3 pos{ p.vertexInfo.Position.x + (m_EmitterSettings.velocity.x * elapsedTime),
					p.vertexInfo.Position.y + (m_EmitterSettings.velocity.y * elapsedTime),
					p.vertexInfo.Position.z + (m_EmitterSettings.velocity.z * elapsedTime) };
	p.vertexInfo.Position = pos;

	// life percent
	const float lifePercent{ p.currentEnergy / p.totalEnergy };

	// color
	XMFLOAT4 color{ m_EmitterSettings.color.x, m_EmitterSettings.color.y,
					m_EmitterSettings.color.z, lifePercent };
	p.vertexInfo.Color = color;

	// size
	float a{ p.initialSize };
	float b{ p.sizeChange + p.initialSize };
	float t{ 1.f - lifePercent };
	float size{ a + t * (b - a) };
	p.vertexInfo.Size = size;
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	//TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	// energy
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	// pos
	auto direction{ XMVectorSet(1.f, 0.f, 0.f, 1.f) };
	auto rotationMat{ XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI)) };
	direction = XMVector3Transform(direction, rotationMat);
	auto worldPos{ GetTransform()->GetWorldPosition() };
	auto originPos{ XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 0.f) };
	float distance{ MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };
	XMStoreFloat3(&p.vertexInfo.Position, originPos + direction * distance);

	// size
	p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.vertexInfo.Size = p.initialSize;
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	// rot
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	// color
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	auto pDeviceContext{ sceneContext.d3dContext.pDeviceContext };
	auto techniqueContext{ m_pParticleMaterial->GetTechniqueContext() };

	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT offset{ 0 };
	UINT strides{ static_cast<UINT>(sizeof(VertexParticle)) };
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &strides, &offset);

	D3DX11_TECHNIQUE_DESC techDesc{};
	techniqueContext.pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		techniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_ActiveParticles, 0, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if (ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}