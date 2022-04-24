#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float lenght)
	: GameObject{}
	, m_pBaseMat{ pMaterial }
	, m_Lenght{ lenght }
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(m_Lenght, 0.f, 0.f);
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	XMFLOAT4X4 worldMatrix{ GetTransform()->GetWorld() };
	XMVECTOR worldDeterminant{ XMMatrixDeterminant(XMLoadFloat4x4(&worldMatrix)) };
	XMMATRIX inverseWorld{ XMMatrixInverse(&worldDeterminant, XMLoadFloat4x4(&worldMatrix)) };
	XMStoreFloat4x4(&m_BindPose, inverseWorld);

	std::vector<BoneObject*> pChilderen{ GetChildren<BoneObject>() };
	for (size_t i = 0; i < pChilderen.size(); i++)
	{
		pChilderen[i]->CalculateBindPose();
	}
}

void BoneObject::Initialize(const SceneContext&)
{
	GameObject* pEmpty{ new GameObject() };
	this->AddChild(pEmpty);
	auto modelComp{ new ModelComponent(L"Meshes/Bone.ovm") };
	modelComp->SetMaterial(m_pBaseMat);
	pEmpty->AddComponent<ModelComponent>(modelComp);
	pEmpty->GetTransform()->Rotate(0.f, -90.f, 0.f);
	pEmpty->GetTransform()->Scale(m_Lenght);
}
