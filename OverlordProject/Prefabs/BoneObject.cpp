#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float lenght)
	: m_pBaseMat{ pMaterial }
	, m_Lenght{ lenght }
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(XMVECTOR{ m_Lenght, 0.f, 0.f });
	this->AddChild(pBone);
}

void BoneObject::Initialize(const SceneContext&)
{
	GameObject* pEmpty{};
	this->AddChild(pEmpty);
	pEmpty->AddComponent(new ModelComponent(L"Meshes/Bone.ovm"));
	pEmpty->GetTransform()->Rotate(XMVECTOR{ 0.f, -90.f, 0.f });
	pEmpty->GetTransform()->Scale(m_Lenght);
}
