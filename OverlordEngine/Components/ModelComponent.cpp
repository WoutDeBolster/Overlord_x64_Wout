#include "stdafx.h"
#include "ModelComponent.h"

ModelComponent::ModelComponent(const std::wstring& assetFile):
	m_AssetFile(assetFile)
{
}

ModelComponent::~ModelComponent()
{
	SafeDelete(m_pAnimator);
}

void ModelComponent::Initialize(const SceneContext& sceneContext)
{
	m_pMeshFilter = ContentManager::Load<MeshFilter>(m_AssetFile);
	m_pMeshFilter->BuildIndexBuffer(sceneContext);

	if (m_pMeshFilter->m_HasAnimations)
		m_pAnimator = new ModelAnimator(m_pMeshFilter);

	if (m_MaterialChanged && m_pMaterial)
	{
		m_pMeshFilter->BuildVertexBuffer(sceneContext, m_pMaterial);
		m_MaterialChanged = false;
	}
}

void ModelComponent::Update(const SceneContext& sceneContext)
{
	if (m_pAnimator)
		m_pAnimator->Update(sceneContext);
};

void ModelComponent::Draw(const SceneContext& sceneContext)
{
	if (!m_pMaterial)
	{
		Logger::LogWarning(L"ModelComponent::Draw() > No BaseMaterial!");
		return;
	}

	m_pMaterial->UpdateEffectVariables(sceneContext, this);

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;

	//Set Inputlayout
	pDeviceContext->IASetInputLayout(m_pMaterial->GetTechniqueContext().pInputLayout);

	//Set Vertex Buffer
	const UINT offset = 0;
	const auto& vertexBufferData = m_pMeshFilter->GetVertexBufferData(sceneContext, m_pMaterial);
	pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
	                                               &offset);

	//Set Index Buffer
	pDeviceContext->IASetIndexBuffer(m_pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DRAW
	auto tech = m_pMaterial->GetTechniqueContext().pTechnique;
	D3DX11_TECHNIQUE_DESC techDesc{};

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ModelComponent::SetMaterial(BaseMaterial* pMaterial)
{
	if(pMaterial == nullptr)
	{
		m_pMaterial = nullptr;
		return;
	}

	if(!pMaterial->HasValidMaterialId())
	{
		Logger::LogWarning(L"BaseMaterial does not have a valid BaseMaterial Id. Make sure to add the material to the material manager first.");
		return;
	}
	
	m_pMaterial = pMaterial;
	m_MaterialChanged = true;

	if(m_IsInitialized && GetScene())
	{

		m_pMeshFilter->BuildVertexBuffer(GetScene()->GetSceneContext(), m_pMaterial);

		m_MaterialChanged = false;
	}
}

void ModelComponent::SetMaterial(UINT materialId)
{
	const auto pMaterial = MaterialManager::Get()->GetMaterial(materialId);
	SetMaterial(pMaterial);
}
