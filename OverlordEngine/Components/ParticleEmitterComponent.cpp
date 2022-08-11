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
	TODO_W9(L"Implement Destructor");
	delete m_ParticlesArray;

	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Initialize");
	if (!m_pParticleMaterial)
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement CreateVertexBuffer");
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_ParticleCount * static_cast<UINT>(sizeof(VertexParticle));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	if (sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pVertexBuffer) != S_OK)
		Logger::LogError(L"ParticleEmitterComponent::CreateVertexBuffer - Error when creating m_pVertexBuffer!");
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement Update");
	float avgEnergy = (m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2.f;
	float particleInterval = avgEnergy / m_ParticleCount;

	float elapsedTime = sceneContext.pGameTime->GetElapsed();
	m_LastParticleSpawn += elapsedTime;

	m_ActiveParticles = 0;
	D3D11_BUFFER_DESC mapDesc{};
	m_pVertexBuffer->GetDesc(&mapDesc);

	D3D11_MAPPED_SUBRESOURCE mappedSubResource{ nullptr };
	if (sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource) != S_OK)
		Logger::LogError(L"ParticleEmitterComponent::Update - Error when mapping m_pVertexBuffer!");

	VertexParticle* pBuffer{ nullptr };
	pBuffer = reinterpret_cast<VertexParticle*>(mappedSubResource.pData);


	for (size_t idx{}; idx < m_ParticleCount; ++idx)
	{
		if (m_ParticlesArray[idx].isActive)
			UpdateParticle(m_ParticlesArray[idx], elapsedTime);

		if (!m_ParticlesArray[idx].isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(m_ParticlesArray[idx]);
			//m_LastParticleSpawn = 0.f;
		}

		if (m_ParticlesArray[idx].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[idx].vertexInfo;
			++m_ActiveParticles;
		}

	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& pParticle, float elapsedTime) const
{
	TODO_W9(L"Implement UpdateParticle");
	if (!pParticle.isActive)
		return;

	pParticle.currentEnergy -= elapsedTime;
	if (pParticle.currentEnergy < 0.f)
	{
		pParticle.isActive = false;
		return;
	}

	pParticle.vertexInfo.Position.x += (m_EmitterSettings.velocity.x * elapsedTime);
	pParticle.vertexInfo.Position.y += (m_EmitterSettings.velocity.y * elapsedTime);
	pParticle.vertexInfo.Position.z += (m_EmitterSettings.velocity.z * elapsedTime);

	float lifePercent = pParticle.currentEnergy / pParticle.totalEnergy;

	pParticle.vertexInfo.Color = m_EmitterSettings.color;
	pParticle.vertexInfo.Color.w = m_EmitterSettings.color.w * lifePercent * 2.f;

	//Lerp
	float min{ pParticle.initialSize };
	float max{ pParticle.sizeChange + min };
	float t{ 1.f - lifePercent };
	pParticle.vertexInfo.Size = min + t * (max - min);
}

void ParticleEmitterComponent::SpawnParticle(Particle& pParticle)
{
	TODO_W9(L"Implement SpawnParticle");
	pParticle.isActive = true;
	pParticle.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	pParticle.currentEnergy = pParticle.totalEnergy;

	XMVECTOR randomDirection = { 1.f, 0.f, 0.f };
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));
	randomDirection = XMVector3TransformNormal(randomDirection, rotationMatrix);

	float randomDistance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);

	XMVECTOR EmitterPos = XMLoadFloat3(&GetTransform()->GetPosition());

	XMStoreFloat3(&(pParticle.vertexInfo.Position), EmitterPos + (randomDirection * randomDistance));

	pParticle.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	pParticle.initialSize = pParticle.vertexInfo.Size;

	pParticle.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	pParticle.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	pParticle.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	TODO_W9(L"Implement PostDraw");
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	MaterialTechniqueContext techniqueContext = m_pParticleMaterial->GetTechniqueContext();

	//TO-DO
	//3. Set the InputLayout
	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	//4. Set the PrimitiveTopology
	//	a.Remember we are only using points for our particles, so select the appropriate topology.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//5. Set the VertexBuffer
	//	a.We have no startslot and only one buffer
	//	b.We have no offset
	//	c.The size of one vertex(stride) is equal to the size of VertexParticle
	const UINT offset{ 0 };
	const UINT stride{ sizeof(VertexParticle) };
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//6. For each pass of our technique
	//	a.Apply the pass
	//	b.Draw the vertices!(dThe number of vertices we want to draw is equal to m_ActiveParticle
	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	techniqueContext.pTechnique->GetDesc(&techniqueDesc);

	for (uint32_t passIdx{}; passIdx < techniqueDesc.Passes; ++passIdx)
	{
		techniqueContext.pTechnique->GetPassByIndex(passIdx)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
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