#include "stdafx.h"
#include "MaterialManager.h"

MaterialManager::~MaterialManager()
{
	for(auto& pMaterial:m_Materials)
	{
		SafeDelete(pMaterial);
	}

	m_Materials.clear();
}

BaseMaterial* MaterialManager::GetMaterial(UINT materialId) const
{
	BaseMaterial* pBase{};
	if (materialId < m_Materials.size())
	{
		pBase = m_Materials[materialId];
		return pBase;
	}

	if (pBase == nullptr)
	{
		Logger::LogWarning(L"Material with ID={} not found", materialId);
	}

	return nullptr;
}

void MaterialManager::RemoveMaterial(UINT materialId, bool deleteObj)
{
	if(!IsValid(materialId) || m_Materials.size() < materialId || !m_Materials[materialId])
	{
		Logger::LogWarning(L"MaterialManager::RemoveMaterial >> Invalid Material ID");
		return;
	}

	m_Materials[materialId]->ResetMaterialId();

	if(deleteObj)
	{
		delete m_Materials[materialId];
	}

	m_Materials[materialId] = nullptr;
}

void MaterialManager::RemoveMaterial(BaseMaterial* pMaterial, bool deleteObj)
{
	const auto materialId = pMaterial->GetMaterialId();
	if (!IsValid(materialId) || m_Materials.size() < materialId || !m_Materials[materialId])
	{
		Logger::LogWarning(L"MaterialManager::RemoveMaterial >> Invalid Material ID");
		return;
	}

	if(m_Materials[materialId] == pMaterial)
	{
		RemoveMaterial(materialId, deleteObj);
	}
}
